#include "SceneRenderer.h"
#include "RendererItem.h"
#include "core/Messenger.h"
#include "graphics/drivers/GraphicsOperation.h"
#include "graphics/drivers/VulkanDriver.h"
#include "scenes/Game.h"
#include <glm/ext/matrix_transform.hpp>
#include <ranges>
#include <vulkan/vulkan_core.h>

namespace SSGE
{

SceneRenderer::SceneRenderer(VulkanDriver *driver, Messenger *messenger)
    : m_driver(driver), m_messenger(messenger), m_camera(driver)
{
    m_messenger->connect<RendererItem::ItemDeletedMessage>(this,
                                                           [this](auto &message) { this->itemRemoved(message.key); });
    init();
}

SceneRenderer::~SceneRenderer()
{
    m_messenger->disconnect(this);
    cleanupGraphicsResources();
}

auto SceneRenderer::init() -> void
{
    m_primitives[GraphicsDriver::ElementType::Quad] = {};
    m_primitives[GraphicsDriver::ElementType::Circle] = {};
    initGraphicsResources();
}

auto SceneRenderer::reset() -> void
{
    for (std::vector<GraphicElement *> elements : m_elementsByType | std::views::values)
    {
        for (auto element : elements)
        {
            element->reset();
        }
    }
    m_items.clear();
    m_addedSet.clear();
    m_removedSet.clear();
}

auto SceneRenderer::render(uint32_t frameIndex, const Resolution &resolution, VkFence fence, VkFramebuffer frameBuffer,
                           VkRenderPass renderPass, const std::vector<VkSemaphore> &waitSemaphores,
                           const std::vector<VkSemaphore> &signalSemaphores) -> void
{
    handleSceneOperations();

    m_camera.update(resolution.width, resolution.height, frameIndex);

    std::array elementTypes = {Rectangle, Circle};

    m_driver->waitForFence(fence);
    m_driver->resetFence(fence);

    // render scene texture
    auto commandBuffer = m_driver->beginCommandWrite(frameIndex);

    RenderInfo renderInfo = VulkanDriver::beginRenderPass(renderPass, commandBuffer, frameBuffer, resolution);

    for (const auto &elementType : elementTypes)
    {
        if (!m_elementsByType.contains(elementType))
            continue;

        // FIXME: this is bug prone, we have two enums doing the same thing here.
        PrimitiveData &primitiveData = m_primitives[static_cast<GraphicsDriver::ElementType>(elementType)];
        auto [pipeline, pipelineLayout] = m_pipelineInfos[static_cast<GraphicsDriver::ElementType>(elementType)];

        VulkanDriver::prepareDraw(commandBuffer, renderInfo, pipeline);

        for (GraphicElement *element : m_elementsByType[elementType])
        {
            if (element->instanceCount == 0)
            {
                continue;
            }

            updateStorageBuffer(element, frameIndex);

            VulkanDriver::drawElementInstances(commandBuffer, element, frameIndex, pipelineLayout, primitiveData);
        }
    }

    VulkanDriver::endRenderPassAndCommandBuffer(commandBuffer);

    m_driver->submitCommandBuffer(commandBuffer, waitSemaphores, signalSemaphores, fence);
}

auto SceneRenderer::initGraphicsResources() -> void
{
    m_descriptorSetLayout =
        m_driver->createDescriptorSetLayout({{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1}});

    m_pipelineInfos[GraphicsDriver::ElementType::Quad] = m_driver->createDefaultGraphicsPipeline(m_descriptorSetLayout);
    m_pipelineInfos[GraphicsDriver::ElementType::Circle] =
        m_driver->createCircleGraphicsPipeline(m_descriptorSetLayout);
}

auto SceneRenderer::cleanupGraphicsResources() -> void
{
    for (const auto &data : m_primitives | std::views::values)
    {
        m_driver->freeMappedBuffer({.buffer = data.indexBuffer, .bufferMemory = data.indexBufferMemory});

        m_driver->freeMappedBuffer({.buffer = data.vertexBuffer, .bufferMemory = data.vertexBufferMemory});
    }

    for (auto &info : m_pipelineInfos | std::views::values)
    {
        m_driver->destroyPipelineInfo(info);
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        for (auto &elements : m_elementsByType | std::views::values)
        {
            for (const auto element : elements)
            {
                m_driver->freeMappedBuffer(element->storageBuffers[i]);
            }
        }
    }

    for (auto &elements : m_elementsByType | std::views::values)
    {
        for (const auto element : elements)
        {
            m_driver->destroyDescriptorPool(element->descriptorPool);
        }
    }

    m_driver->destroyDescriptorSetLayout(m_descriptorSetLayout);

    for (auto &elements : m_elementsByType | std::views::values)
    {
        for (const auto element : elements)
        {
            delete element;
        }
    }
}

std::map<RendererItem *, GraphicsOperation> SceneRenderer::getAddOrRemoveOperations()
{
    std::map<RendererItem *, GraphicsOperation> addOrRemoveOperations;
    if (m_addedSet.empty() && m_removedSet.empty())
        return addOrRemoveOperations;

    for (RendererItem *added : m_addedSet)
    {
        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Add;
        operation.item = added;
        addOrRemoveOperations.insert(std::make_pair(added, operation));
    }

    for (const auto removed : m_removedSet)
    {
        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Remove;
        operation.key = removed;
        addOrRemoveOperations.insert(std::make_pair(nullptr, operation));
    }

    m_addedSet.clear();
    m_removedSet.clear();

    return addOrRemoveOperations;
}

std::vector<GraphicsOperation> SceneRenderer::getUpdateOperations()
{
    std::vector<GraphicsOperation> updateOperations;

    for (auto& [key, item] : m_items)
    {
        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Update;
        operation.item = item;
        operation.key = key;
        updateOperations.push_back(operation);
    }

    return updateOperations;
}

auto SceneRenderer::handleSceneOperations() -> void
{
    auto addOrRemoveOperations = getAddOrRemoveOperations();

    if (!addOrRemoveOperations.empty())
        m_driver->waitIdle();

    for (auto &operation : getUpdateOperations())
    {
        performOperation(&operation);
    }

    for (auto &[renderItem, operation] : addOrRemoveOperations)
    {
        performOperation(&operation);

        if (!operation.result.has_value() || operation.result.value() < 0)
            continue;

        if (operation.type == GraphicsOperation::Type::Add)
        {
            renderItem->setKey(operation.result.value());
            m_items.insert(std::make_pair(operation.result.value(), renderItem));
        }
    }
}

void SceneRenderer::updateStorageBuffer(const GraphicElement *element, const uint32_t currentImage)
{
    size_t offset = 0;
    size_t instancesCopied = 0;

    while (offset < MAX_INSTANCES && instancesCopied < element->instanceCount)
    {
        while (!element->instanceUsed[offset])
        {
            ++offset;
        }
        size_t start = offset;
        while (element->instanceUsed[offset])
        {
            ++offset;
        }
        size_t end = offset - 1;
        size_t instanceCountToCopy = end - start + 1;
        size_t destBufferStart = instancesCopied;
        instancesCopied += instanceCountToCopy;
        auto mappedBuffer = reinterpret_cast<uintptr_t>(element->storageBuffers[currentImage].bufferMapped);
        mappedBuffer += sizeof(InstanceData) * destBufferStart;
        memcpy(reinterpret_cast<void *>(mappedBuffer), element->instanceData.data() + start,
               sizeof(InstanceData) * instanceCountToCopy);
    }
}

auto SceneRenderer::addItem(RendererItem *item) -> void
{
    m_addedSet.insert(item);
}

void SceneRenderer::performOperation(GraphicsOperation *operation)
{
    operation->result = -1;
    GraphicElement *element;
    switch (operation->type)
    {
    case GraphicsOperation::Type::Add: {
        RendererItem *item = operation->item.value();
        auto itemType = item->getType();
        if (m_elementsByType.contains(itemType))
        {
            element = m_elementsByType[itemType].back();
        }
        else
        {
            element = new GraphicElement{
                .type = static_cast<GraphicsDriver::ElementType>(itemType),
                .descriptorPool = VK_NULL_HANDLE,
                .descriptorSets = {},
                .storageBuffers = {},
                .instanceData = {},
                .instanceUsed = {},
                .instanceCount = 0,
            };

            m_elementsByType[itemType] = std::vector<GraphicElement *>();
            m_elementsByType[itemType].push_back(element);

            element->descriptorPool = m_driver->createDescriptorPool(
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER}, MAX_FRAMES_IN_FLIGHT);

            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                element->storageBuffers[i] = m_driver->createMappedBuffer(sizeof(InstanceData) * MAX_INSTANCES,
                                                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            }

            {
                std::vector<VkDescriptorSet> descriptorSets = m_driver->createDescriptorSets(
                    MAX_FRAMES_IN_FLIGHT, element->descriptorPool, {m_descriptorSetLayout, m_descriptorSetLayout});
                size_t i = 0;
                for (VkDescriptorSet descriptorSet : descriptorSets)
                {
                    element->descriptorSets[i] = descriptorSet;
                    i++;
                }
            }

            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                m_driver->writeDescriptorSet(element->descriptorSets[i], m_camera.getBuffer(i),
                                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
                m_driver->writeDescriptorSet(element->descriptorSets[i], element->storageBuffers[i],
                                             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);
            }
        }
        glm::mat4 model = glm::translate(glm::mat4(1.0f), item->getTransformPosition());
        model = glm::scale(model, item->getTransformScale());
        model = item->getWorldTransform() * model;
        uint32_t index = element->addInstance({.model = model, .inColor = item->getFillColor()});
        PrimitiveData &primitiveData = m_primitives[static_cast<GraphicsDriver::ElementType>(itemType)];
        m_driver->updateVertexBuffer(element, primitiveData);
        m_driver->updateIndexBuffer(element, primitiveData);
        operation->result = index + (static_cast<size_t>(itemType) * MAX_INSTANCES);
    }
    break;
    case GraphicsOperation::Type::Remove: {
        operation->result = 0;
        RendererItemType type = static_cast<RendererItemType>(operation->key / MAX_INSTANCES);
        element = m_elementsByType[type].back();
        uint32_t index = operation->key - (static_cast<size_t>(type) * MAX_INSTANCES);
        element->removeInstance(index);
        break;
    }
    case GraphicsOperation::Type::Update: {
        auto item = operation->item.value();
        auto itemType = item->getType();
        element = m_elementsByType[itemType].back();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), item->getTransformPosition());
        model = glm::scale(model, item->getTransformScale());
        model = item->getWorldTransform() * model;
        const size_t instanceIndex =
            static_cast<size_t>(operation->key) - (static_cast<size_t>(itemType) * MAX_INSTANCES);

        auto &[instanceModel, instanceColor] = element->instanceData[instanceIndex];
        instanceModel = model;
        instanceColor = item->getFillColor();
        operation->result = 0;
    }
    break;
    default:
        break;
    }
}

auto SceneRenderer::itemRemoved(uint32_t key) -> void
{
    m_items.erase(key);
    m_removedSet.insert(key);
}

} // namespace SSGE
