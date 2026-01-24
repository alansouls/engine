#include "Renderer.h"

#include "graphics/drivers/shaders/shaders.h"
#include "scene/RendererItem.h"

namespace SSGE
{
Renderer::Renderer(EngineWindow *window, const RendererOptions &options)
    : m_driver(VK_NULL_HANDLE), m_window(window), m_options(options), m_currentImage(0)
{
    initWindow(window);
    initGraphicsDriver();
}

auto Renderer::render() -> void
{
    preRender(m_currentImage);
    auto frameFence = m_driver->getFrameFence(m_currentImage);
    m_driver->waitForFence(frameFence);
    drawFrame(m_currentImage);
    m_currentImage = (m_currentImage + 1) % MAX_FRAMES_IN_FLIGHT;
    postRender(m_currentImage);
}

auto Renderer::addItem(RendererItem *item) const -> void
{
    for (SceneRenderer *sceneRenderer : m_sceneRenderers)
    {
        sceneRenderer->addItem(item);
    }
}

auto Renderer::getWidth() const -> uint32_t
{
    return m_window->getSize().width;
}

auto Renderer::getHeight() const -> uint32_t
{
    return m_window->getSize().height;
}

auto Renderer::initGraphicsDriver() -> void
{
    const std::vector validationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef MACOSX
                                                        "VK_KHR_portability_subset"
#endif
    };

    const auto vertexShader = Shaders::findShader("shader.vert");
    const auto fragmentShader = Shaders::findShader("shader.frag");
    const auto circleVertexShader = Shaders::findShader("CircleShader.vert");
    const auto circleFragmentShader = Shaders::findShader("CircleShader.frag");

    m_driver = std::make_unique<VulkanDriver>(
        getVulkanRequiredExtensions(), validationLayers, deviceExtensions, m_window->getWindow(),
        GraphicsDriverOptions{m_options.debugModeOn, vertexShader->data, vertexShader->size, fragmentShader->data,
                              fragmentShader->size, circleVertexShader->data, circleVertexShader->size,
                              circleFragmentShader->data, circleFragmentShader->size});
}

auto Renderer::preRender(uint32_t currentFrame) -> void
{
}

auto Renderer::postRender(uint32_t currentFrame) -> void
{
}

auto Renderer::initWindow(EngineWindow *mainWindow) -> void
{
    GLFWwindow *glfwWindow = mainWindow->getWindow();
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, framebufferResizeCallback);
}

std::vector<const char *> Renderer::getVulkanRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

void Renderer::framebufferResizeCallback(GLFWwindow *window, int, int)
{
    const auto renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->m_driver->windowResized();
}

} // namespace SSGE