#include "Renderer.h"

#include "../drivers/VulkanDriver.h"
#include "../drivers/shaders/shaders.h"
#include "scene/EditorSceneRenderer.h"
#include <stdexcept>
#include <vector>

Renderer::Renderer(EngineWindow *mainWindow, const RendererOptions &options)
    : m_window(mainWindow), m_options(options), m_driver(VK_NULL_HANDLE), m_width(0), m_height(0), m_currentImage(0)
{
    initWindow(mainWindow);
    initGraphicsDriver();
    m_editorSceneRenderer = std::make_unique<EditorSceneRenderer>(m_driver.get(), 0, 0);
    m_editorSceneRenderer2 = std::make_unique<EditorSceneRenderer>(m_driver.get(), 0, 0);
    m_sceneRenderers = {m_editorSceneRenderer->getRenderer()};
    m_uiRenderer = std::make_unique<UIRenderer>(mainWindow, m_driver.get());
    m_uiRenderer->init(m_editorSceneRenderer.get(), m_editorSceneRenderer2.get());
}

Renderer::~Renderer()
{
    m_driver->waitIdle();
}

void Renderer::render()
{
    ImDrawData *data = m_uiRenderer->renderUI(m_currentImage);
    auto frameFence = m_driver->getFrameFence(m_currentImage);
    m_driver->waitForFence(frameFence);
    m_driver->drawFrame(m_currentImage, data);
    m_currentImage = (m_currentImage + 1) % MAX_FRAMES_IN_FLIGHT;
}

auto Renderer::getWidth() const -> uint32_t
{
    return m_width;
}

auto Renderer::getHeight() const -> uint32_t
{
    return m_height;
}

auto Renderer::getSceneWidth() const -> uint32_t
{
    return m_editorSceneRenderer->getWidth();
}

auto Renderer::getSceneHeight() const -> uint32_t
{
    return m_editorSceneRenderer->getHeight();
}

void Renderer::framebufferResizeCallback(GLFWwindow *window, int, int)
{
    const auto renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    renderer->setDimensions();
    renderer->m_driver->windowResized();
}

void Renderer::setDimensions()
{
    EngineWindow::WindowSize size = m_window->getSize();
    m_width = size.width;
    m_height = size.height;
}

auto Renderer::addItem(RendererItem *item) const -> void
{
    m_editorSceneRenderer->addItem(item);
    m_editorSceneRenderer2->addItem(item);
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

auto Renderer::initWindow(EngineWindow *mainWindow) -> void
{
    GLFWwindow *glfwWindow = mainWindow->getWindow();
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, framebufferResizeCallback);
    setDimensions();
}

std::vector<const char *> Renderer::getVulkanRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}
