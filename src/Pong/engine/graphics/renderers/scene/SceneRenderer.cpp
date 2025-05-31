#include "SceneRenderer.h"
#include "../../drivers/GraphicsOperation.h"
#include "CircleItem.h"
#include "RectangleItem.h"
#include "RendererItem.h"
#include "engine/graphics/utils/UniformBufferObject.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <ranges>

SceneRenderer::SceneRenderer(VulkanDriver *driver, uint32_t width, uint32_t height)
    : m_driver(driver), m_renderPass(VK_NULL_HANDLE), m_framebuffers({VK_NULL_HANDLE}), m_cameras({}),
      m_resizeWidth(-1), m_resizeHeight(-1)
{
    init(width, height);
}

SceneRenderer::~SceneRenderer()
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

        m_driver->freeMappedBuffer(m_cameras[i]);
        m_driver->destroyFrameBuffer(m_framebuffers[i]);
    }

    for (auto &elements : m_elementsByType | std::views::values)
    {
        for (const auto element : elements)
        {
            m_driver->destroyDescriptorPool(element->descriptorPool);
        }
    }

    m_driver->destroyRenderPass(m_renderPass);
    m_driver->destroyDescriptorSetLayout(m_descriptorSetLayout);
}

auto SceneRenderer::render(const uint32_t currentImage) -> std::shared_ptr<SceneImage>
{
    handleSceneOperations();

    updateCameraBuffer(currentImage);

    auto &image = m_images[currentImage];

    std::array elementTypes = {GraphicsDriver::ElementType::Quad, GraphicsDriver::ElementType::Circle};

    // render scene texture
    auto commandBuffer = m_driver->beginCommandWrite(currentImage);

    if (m_resizeWidth[currentImage] != -1 && m_resizeHeight[currentImage] != -1)
    {
        commitResize(currentImage);
    }

    RenderInfo renderInfo = VulkanDriver::beginRenderPass(m_renderPass, commandBuffer, m_framebuffers[currentImage],
                                                          {image->getWidth(), image->getHeight()});

    for (const auto &elementType : elementTypes)
    {
        if (!m_elementsByType.contains(elementType))
            continue;

        m_driver->prepareDraw(commandBuffer, elementType, renderInfo);

        for (auto element : m_elementsByType[elementType])
        {
            updateStorageBuffer(element, currentImage);

            m_driver->drawElementInstances(commandBuffer, element, currentImage);
        }
    }

    VulkanDriver::endRenderPassAndCommandBuffer(commandBuffer);

    m_driver->submitCommandBuffer(commandBuffer, {}, {}, currentImage);
    //

    return image;
}

auto SceneRenderer::resize(const uint32_t width, const uint32_t height) -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_resizeWidth[i] = width;
        m_resizeHeight[i] = height;
    }
}

auto SceneRenderer::commitResize(uint32_t currentImage) -> void
{
    const auto &image = m_images[currentImage];

    image->resize(m_resizeWidth[currentImage], m_resizeHeight[currentImage]);

    m_driver->destroyFrameBuffer(m_framebuffers[currentImage]);

    m_framebuffers[currentImage] =
        m_driver->createFrameBuffer(m_renderPass, image->getImageView(), image->getWidth(), image->getHeight());

    m_resizeWidth[currentImage] = -1;
    m_resizeHeight[currentImage] = -1;
}

auto SceneRenderer::addItem(RendererItem *item) -> void
{
    item->addCallback(this, &itemUpdated);
    m_addedSet.insert(item);
}

auto SceneRenderer::getWidth() const -> uint32_t
{
    // TODO should we improve this?
    return m_images[0]->getWidth();
}

auto SceneRenderer::getHeight() const -> uint32_t
{
    // TODO should we improve this?
    return m_images[0]->getHeight();
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

auto SceneRenderer::init(uint32_t width, uint32_t height) -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_images[i] = std::make_shared<SceneImage>(width, height, m_driver);
    }

    m_renderPass = m_driver->createRenderPass(VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_SRGB,
                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_descriptorSetLayout =
        m_driver->createDescriptorSetLayout({{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1}});

    m_driver->createDefaultGraphicsPipeline(m_descriptorSetLayout);
    m_driver->createCircleGraphicsPipeline(m_descriptorSetLayout);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_cameras[i] = m_driver->createMappedBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        m_framebuffers[i] = m_driver->createFrameBuffer(m_renderPass, m_images[i]->getImageView(),
                                                        m_images[i]->getWidth(), m_images[i]->getHeight());
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
        operation.transformPosition = added->getTransformPosition();
        operation.transformScale = added->getTransformScale();
        switch (added->getType())
        {
        case RendererItem::RendererItemType::Rectangle:
            operation.elementType = GraphicsDriver::ElementType::Quad;
            operation.color = reinterpret_cast<RectangleItem *>(added)->getFillColor();
            break;
        case RendererItem::RendererItemType::Circle:
            operation.elementType = GraphicsDriver::ElementType::Circle;
            operation.color = reinterpret_cast<CircleItem *>(added)->getFillColor();
            break;
        default:
            throw std::runtime_error("Renderer item type not supported");
        }
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
        switch (updated->getType())
        {
        case RendererItem::RendererItemType::Rectangle:
            operation.elementType = GraphicsDriver::ElementType::Quad;
            operation.color = reinterpret_cast<RectangleItem *>(updated)->getFillColor();
            break;
        case RendererItem::RendererItemType::Circle:
            operation.elementType = GraphicsDriver::ElementType::Circle;
            operation.color = reinterpret_cast<CircleItem *>(updated)->getFillColor();
            break;
        default:
            throw std::runtime_error("Renderer item type not supported");
        }
        operation.transformPosition = updated->getTransformPosition();
        operation.transformScale = updated->getTransformScale();
        operation.key = key;
        updateOperations.push_back(operation);
    }

    m_updatedSet.clear();

    return updateOperations;
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
        if (m_elementsByType.contains(operation->elementType.value()))
        {
            element = m_elementsByType[operation->elementType.value()].back();
        }
        else
        {
            // ReSharper disable once CppDFAMemoryLeak - this is freed in the cleanup function
            element = new GraphicElement{
                .type = operation->elementType.value(),
            };

            m_elementsByType[operation->elementType.value()] = std::vector<GraphicElement *>();
            m_elementsByType[operation->elementType.value()].push_back(element);

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
                m_driver->writeDescriptorSet(element->descriptorSets[i], m_cameras[i],
                                             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0);
                m_driver->writeDescriptorSet(element->descriptorSets[i], element->storageBuffers[i],
                                             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1);
            }
        }
        glm::mat4 model = glm::translate(glm::mat4(1.0f), operation->transformPosition.value());
        model = glm::scale(model, operation->transformScale.value());
        element->instanceData.push_back({.model = model, .inColor = operation->color.value()});
        m_driver->updateVertexBuffer(element);
        m_driver->updateIndexBuffer(element);
        operation->result =
            element->instanceData.size() + (static_cast<size_t>(operation->elementType.value()) * MAX_INSTANCES);
    }
    break;
    case GraphicsOperation::Type::Remove:
        operation->result = 0;
        // TODO
        break;
    case GraphicsOperation::Type::Update: {
        element = m_elementsByType[operation->elementType.value()].back();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), operation->transformPosition.value());
        model = glm::scale(model, operation->transformScale.value());
        const size_t instanceIndex = static_cast<size_t>(operation->key) - 1 -
                                     (static_cast<size_t>(operation->elementType.value()) * MAX_INSTANCES);

        auto &[instanceModel, instanceColor] = element->instanceData[instanceIndex];
        instanceModel = model;
        instanceColor = operation->color.value();
        operation->result = 0;
    }
    break;
    default:
        break;
    }
}

auto SceneRenderer::updateCameraBuffer(const uint32_t currentImage) const -> void
{
    const SceneImage *image = m_images[currentImage].get();
    UniformBufferObject ubo{};

    ubo.view = glm::mat4(1.0f);
    ubo.proj = glm::ortho(0.0f, static_cast<float>(image->getWidth()), 0.0f, static_cast<float>(image->getHeight()),
                          -1000.0f, 1000.0f);

    memcpy(m_cameras[currentImage].bufferMapped, &ubo, sizeof(ubo));
}

void SceneRenderer::updateStorageBuffer(const GraphicElement *element, const uint32_t currentImage)
{
    const InstanceData *instanceDataArray = element->instanceData.data();
    const size_t instanceDataSize = element->instanceData.size();

    memcpy(element->storageBuffers[currentImage].bufferMapped, instanceDataArray,
           instanceDataSize * sizeof(InstanceData));
}
