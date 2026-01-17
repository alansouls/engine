#include "SceneRenderer.h"
#include "RendererItem.h"
#include "graphics/drivers/GraphicsOperation.h"
#include <glm/ext/matrix_transform.hpp>
#include <ranges>

namespace SSGE
{

SceneRenderer::SceneRenderer(VulkanDriver *driver) : m_driver(driver), m_camera(driver)
{
    initGraphicsResources();
}

SceneRenderer::~SceneRenderer()
{
    cleanupGraphicsResources();
}

auto SceneRenderer::render(uint32_t frameIndex, const Resolution &resolution, VkFence fence, VkFramebuffer frameBuffer, VkRenderPass renderPass,
                           const std::vector<VkSemaphore> &waitSemaphores,
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

        m_driver->prepareDraw(commandBuffer, static_cast<GraphicsDriver::ElementType>(elementType), renderInfo);

        for (auto element : m_elementsByType[elementType])
        {
            updateStorageBuffer(element, frameIndex);

            m_driver->drawElementInstances(commandBuffer, element, frameIndex);
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

    m_driver->createDefaultGraphicsPipeline(m_descriptorSetLayout);
    m_driver->createCircleGraphicsPipeline(m_descriptorSetLayout);
}

auto SceneRenderer::cleanupGraphicsResources() -> void
{
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

    if (m_updatedSet.empty())
        return updateOperations;

    for (auto key : m_updatedSet)
    {
        auto updated = m_items.find(key)->second;
        if (updated == nullptr)
            continue;

        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Update;
        operation.item = updated;
        operation.key = key;
        updateOperations.push_back(operation);
    }

    m_updatedSet.clear();

    return updateOperations;
}

auto SceneRenderer::handleSceneOperations() -> void
{
    auto addOrRemoveOperations = getAddOrRemoveOperations();

    if (!addOrRemoveOperations.empty())
        m_driver->waitIdle();

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
        else if (operation.type == GraphicsOperation::Type::Remove)
        {
            m_items.erase(operation.key);
        }
    }

    for (auto &operation : getUpdateOperations())
    {
        if (operation.type != GraphicsOperation::Type::Update)
        {
            throw std::runtime_error("Draw frame accepts only update operations!");
        }
        performOperation(&operation);
    }
}

void SceneRenderer::updateStorageBuffer(const GraphicElement *element, const uint32_t currentImage)
{
    const InstanceData *instanceDataArray = element->instanceData.data();
    const size_t instanceDataSize = element->instanceData.size();

    memcpy(element->storageBuffers[currentImage].bufferMapped, instanceDataArray,
           instanceDataSize * sizeof(InstanceData));
}

auto SceneRenderer::addItem(RendererItem *item) -> void
{
    item->addCallback(this, &itemUpdated);
    m_addedSet.insert(item);
}

auto SceneRenderer::itemUpdated(void *thisPtr, uint32_t itemKey) -> void
{
    const auto renderer = static_cast<SceneRenderer *>(thisPtr);
    if (itemKey == 0)
    {
        return;
    }
    renderer->m_updatedSet.insert(itemKey);
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
            // ReSharper disable once CppDFAMemoryLeak - this is freed in the cleanup function
            element = new GraphicElement{.type = static_cast<GraphicsDriver::ElementType>(itemType)};

            m_elementsByType[itemType] = std::vector<GraphicElement *>();
            m_elementsByType[itemType].push_back(element);

            element->descriptorPool = m_driver->createDescriptorPool(
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER}, MAX_FRAMES_IN_FLIGHT);

            element->storageBuffers.resize(MAX_FRAMES_IN_FLIGHT);

            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {
                element->storageBuffers[i] = m_driver->createMappedBuffer(sizeof(InstanceData) * MAX_INSTANCES,
                                                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
            }

            element->descriptorSets = m_driver->createDescriptorSets(MAX_FRAMES_IN_FLIGHT, element->descriptorPool,
                                                                     {m_descriptorSetLayout, m_descriptorSetLayout});

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
        element->instanceData.push_back({.model = model, .inColor = item->getFillColor()});
        m_driver->updateVertexBuffer(element);
        m_driver->updateIndexBuffer(element);
        operation->result = element->instanceData.size() + (static_cast<size_t>(itemType) * MAX_INSTANCES);
    }
    break;
    case GraphicsOperation::Type::Remove:
        operation->result = 0;
        // TODO
        break;
    case GraphicsOperation::Type::Update: {
        auto item = operation->item.value();
        auto itemType = item->getType();
        element = m_elementsByType[itemType].back();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), item->getTransformPosition());
        model = glm::scale(model, item->getTransformScale());
        model = item->getWorldTransform() * model;
        const size_t instanceIndex =
            static_cast<size_t>(operation->key) - 1 - (static_cast<size_t>(itemType) * MAX_INSTANCES);

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

} // namespace SSGE
