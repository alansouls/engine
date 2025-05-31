#include "Renderer.h"

#include "../drivers/VulkanDriver.h"
#include "../drivers/shaders/shaders.h"
#include "scene/RendererItem.h"
#include "scene/SceneRenderer.h"
#include <stdexcept>
#include <vector>

Renderer::Renderer(EngineWindow *mainWindow, const RendererOptions &options)
    : m_window(mainWindow), m_options(options), m_width(0), m_height(0), m_currentImage(0)
{
    auto glfwWindow = mainWindow->getWindow();
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, Renderer::framebufferResizeCallback);
    setDimensions();
    initGraphicsDriver();
    m_uiRenderer = std::make_unique<UIRenderer>(mainWindow, m_driver);
    m_sceneRenderer = std::make_unique<SceneRenderer>(m_driver, m_width, m_height);
    m_uiRenderer->init(m_sceneRenderer.get());
}

Renderer::~Renderer()
{
    m_driver->waitIdle();
    UIRenderer::cleanup();
    m_driver->cleanup();
}

void Renderer::render()
{
    ImDrawData *data = m_uiRenderer->renderUI(m_currentImage);
    m_driver->drawFrame(m_currentImage, data);
    m_currentImage = (m_currentImage + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::framebufferResizeCallback(GLFWwindow *window, int width, int height)
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

void Renderer::addItem(RendererItem *item) const
{
    m_sceneRenderer->addItem(item);
}

void Renderer::initGraphicsDriver()
{
    const std::vector<const char *> validationLayers = {
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

    m_driver = new VulkanDriver(
        getVulkanRequiredExtensions(), validationLayers, deviceExtensions, m_window->getWindow(),
        GraphicsDriverOptions{m_options.debugModeOn, vertexShader->data, vertexShader->size, fragmentShader->data,
                              fragmentShader->size, circleVertexShader->data, circleVertexShader->size,
                              circleFragmentShader->data, circleFragmentShader->size});

    m_driver->init();
}

std::vector<const char *> Renderer::getVulkanRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}
