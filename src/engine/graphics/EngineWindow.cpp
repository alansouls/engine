#include "EngineWindow.h"

#include <backends/imgui_impl_glfw.h>
#include <stdexcept>

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
    std::cout << "Finshed!\n";
#endif // LOG_ENABLED
}

EngineWindow::~EngineWindow()
{
    glfwSetFramebufferSizeCallback(m_window, nullptr);
    glfwSetKeyCallback(m_window, nullptr);
    glfwSetMouseButtonCallback(m_window, nullptr);
    glfwSetCursorPosCallback(m_window, nullptr);
    glfwSetScrollCallback(m_window, nullptr);

    glfwDestroyWindow(m_window);
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
    auto engineWindow = static_cast<EngineWindow *>(glfwGetWindowUserPointer(window));
    if (!engineWindow)
        return;

    if (engineWindow->m_keyCallback)
        engineWindow->m_keyCallback(key, scancode, action, mods);
}

auto EngineWindow::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) -> void
{
    auto engineWindow = static_cast<EngineWindow *>(glfwGetWindowUserPointer(window));
    if (!engineWindow)
        return;

    if (engineWindow->m_mouseButtonCallback)
        engineWindow->m_mouseButtonCallback(button, action, mods);
}

auto EngineWindow::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) -> void
{
    auto engineWindow = static_cast<EngineWindow *>(glfwGetWindowUserPointer(window));
    if (!engineWindow)
        return;

    if (engineWindow->m_cursorPositionCallback)
        engineWindow->m_cursorPositionCallback(xpos, ypos);
}

auto EngineWindow::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) -> void
{
    auto engineWindow = static_cast<EngineWindow *>(glfwGetWindowUserPointer(window));
    if (!engineWindow)
        return;

    if (engineWindow->m_scrollCallback)
        engineWindow->m_scrollCallback(xoffset, yoffset);
}

auto EngineWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) -> void
{
    auto engineWindow = static_cast<EngineWindow *>(glfwGetWindowUserPointer(window));
    if (!engineWindow)
        return;

    if (engineWindow->m_framebufferResizeCallback)
        engineWindow->m_framebufferResizeCallback(width, height);
}
