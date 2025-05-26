#include "Renderer.h"

#include "../drivers/VulkanDriver.h"
#include "../drivers/shaders/shaders.h"
#include "CircleItem.h"
#include "RectangleItem.h"
#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

Renderer::Renderer(EngineWindow *mainWindow, const RendererOptions &options) : m_window(mainWindow), m_options(options)
{
    auto glfwWindow = mainWindow->getWindow();
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, Renderer::framebufferResizeCallback);
    setDimensions();
    initGraphicsDriver();
    m_uiRenderer = new UIRenderer(mainWindow, reinterpret_cast<VulkanDriver *>(m_driver));
    m_uiRenderer->init();
}

Renderer::~Renderer()
{
    m_driver->waitIdle();
    m_uiRenderer->cleanup();
    m_driver->cleanup();
}

void Renderer::render()
{
    auto addOrRemoveOperations = getAddOrRemoveOperations();

    if (!addOrRemoveOperations.empty())
        m_driver->waitIdle();

    for (auto item : addOrRemoveOperations)
    {
        auto operation = item.second;
        auto renderItem = item.first;
        m_driver->performOperation(&operation);

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

    std::vector<GraphicsOperation> updateOperations = getUpdateOperations();
    std::vector<GraphicsOperation *> operations;
    for (auto &operation : updateOperations)
    {
        operations.push_back(&operation);
    }
    m_uiRenderer->renderUI();
    m_driver->drawFrame(operations);
}

void Renderer::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto renderer = reinterpret_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->setDimensions();
    renderer->m_driver->windowResized();

    for (auto item : renderer->m_items)
    {
        item.second->updateTransform();
        renderer->m_updatedSet.insert(item.first);
    }
}

void Renderer::setDimensions()
{
    EngineWindow::WindowSize size = m_window->getSize();
    m_width = size.width;
    m_height = size.height;
}

glm::vec3 Renderer::getResizeScale(float width, float height) const
{
    EngineWindow::WindowSize size = m_window->getSize();
    glm::vec2 normalizedSize = {width / size.width * 2, height / size.height * 2};
    return glm::vec3(normalizedSize, 1.0f);
}

void Renderer::addItem(RendererItem *item)
{
    item->addCallback(this, &itemUpdated);
    m_addedSet.insert(item);
}

void Renderer::itemUpdated(void *thisPtr, uint32_t itemKey)
{
    auto renderer = reinterpret_cast<Renderer *>(thisPtr);
    if (itemKey == 0)
    {
        return;
    }
    renderer->m_updatedSet.insert(itemKey);
}

void Renderer::initGraphicsDriver()
{
    if (m_options.type == RendererOptions::Vulkan)
    {
        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };

        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef MACOSX
                                                            "VK_KHR_portability_subset"
#endif
        };

        auto vertexShader = Shaders::findShader("shader.vert");
        auto fragmentShader = Shaders::findShader("shader.frag");
        auto circleVertexShader = Shaders::findShader("CircleShader.vert");
        auto circleFragmentShader = Shaders::findShader("CircleShader.frag");

        m_driver = new VulkanDriver(
            getVulkanRequiredExtensions(), validationLayers, deviceExtensions, m_window->getWindow(),
            GraphicsDriverOptions{m_options.debugModeOn, vertexShader->data, vertexShader->size, fragmentShader->data,
                                  fragmentShader->size, circleVertexShader->data, circleVertexShader->size,
                                  circleFragmentShader->data, circleFragmentShader->size});
    }
    else
    {
        throw std::runtime_error("Renderer type not supported");
    }

    m_driver->init();
}

std::vector<const char *> Renderer::getVulkanRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

std::map<RendererItem *, GraphicsOperation> Renderer::getAddOrRemoveOperations()
{
    std::map<RendererItem *, GraphicsOperation> addOrRemoveOperations;
    if (m_addedSet.empty() && m_removedSet.empty())
        return addOrRemoveOperations;

    for (auto added : m_addedSet)
    {
        GraphicsOperation operation;
        operation.type = GraphicsOperation::Type::Add;
        operation.vertexData = added->getVertexData();
        operation.vertexDataSize = added->getVertexDataSize();
        operation.indices = added->getIndices();
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
            std::runtime_error("Renderer item type not supported");
        }
        addOrRemoveOperations.insert(std::make_pair(added, operation));
    }

    for (auto removed : m_removedSet)
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

std::vector<GraphicsOperation> Renderer::getUpdateOperations()
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
