#include "EngineWindow.h"

#include <backends/imgui_impl_glfw.h>
#include <stdexcept>

EngineWindow *EngineWindow::s_mainWindow = nullptr;

EngineWindow::EngineWindow(const EngineWindowProperties &properties)
{
#ifdef LOG_ENABLED
    std::cout << "Initializing window...\n";
#endif // LOG_ENABLED
    // TODO: For some reason I have to call glfwInit in here, that will be a problem with multiple windows
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(properties.width, properties.height, properties.title.data(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPositionCallback);
    glfwSetScrollCallback(m_window, scrollCallback);

#ifdef LOG_ENABLED
    std::cout << "Finished!\n";
#endif // LOG_ENABLED

    if (properties.isMain)
    {
        s_mainWindow = this;
    }
}

EngineWindow::~EngineWindow()
{
    glfwSetFramebufferSizeCallback(m_window, nullptr);
    glfwSetKeyCallback(m_window, nullptr);
    glfwSetMouseButtonCallback(m_window, nullptr);
    glfwSetCursorPosCallback(m_window, nullptr);
    glfwSetScrollCallback(m_window, nullptr);

    glfwDestroyWindow(m_window);

    if (this == s_mainWindow)
    {
        s_mainWindow = nullptr;
    }
}

auto EngineWindow::getSize() const -> WindowSize
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return {width, height};
}

GLFWwindow *EngineWindow::getWindow()
{
    return m_window;
}

auto EngineWindow::initForUI() const -> void
{
    ImGui_ImplGlfw_InitForVulkan(m_window, true);
    ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true);
}

auto EngineWindow::cleanupForUI() -> void
{
    ImGui_ImplGlfw_Shutdown();
}

auto EngineWindow::beginUIFrame() -> void
{
    ImGui_ImplGlfw_NewFrame();
}

auto EngineWindow::endUIFrame() -> void
{
}

auto EngineWindow::setKeyCallback(const std::function<void(int, int, int, int)> &callback) -> void
{
    m_keyCallback = callback;
}

auto EngineWindow::setMouseButtonCallback(const std::function<void(int, int, int)> &callback) -> void
{
    m_mouseButtonCallback = callback;
}

auto EngineWindow::setCursorPositionCallback(const std::function<void(double, double)> &callback) -> void
{
    m_cursorPositionCallback = callback;
}

auto EngineWindow::setScrollCallback(const std::function<void(double, double)> &callback) -> void
{
    m_scrollCallback = callback;
}

auto EngineWindow::setFramebufferResizeCallback(const std::function<void(int, int)> &callback) -> void
{
    m_framebufferResizeCallback = callback;
}

auto EngineWindow::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) -> void
{
    (void)window;
    if (s_mainWindow->m_keyCallback)
        s_mainWindow->m_keyCallback(key, scancode, action, mods);
}

auto EngineWindow::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) -> void
{
    (void)window;
    if (s_mainWindow->m_mouseButtonCallback)
        s_mainWindow->m_mouseButtonCallback(button, action, mods);
}

auto EngineWindow::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) -> void
{
    (void)window;
    if (s_mainWindow->m_cursorPositionCallback)
        s_mainWindow->m_cursorPositionCallback(xpos, ypos);
}

auto EngineWindow::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) -> void
{
    (void)window;
    if (s_mainWindow->m_scrollCallback)
        s_mainWindow->m_scrollCallback(xoffset, yoffset);
}

auto EngineWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) -> void
{
    (void)window;
    if (s_mainWindow->m_framebufferResizeCallback)
        s_mainWindow->m_framebufferResizeCallback(width, height);
}
