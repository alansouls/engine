#include "Renderer.h"

#include "../drivers/VulkanDriver.h"
#include "../drivers/shaders/shaders.h"
#include "scene/SceneRenderer.h"
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
    m_sceneRenderer->render();
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
