#include "EngineWindow.h"
#include <backends/imgui_impl_glfw.h>

EngineWindow::EngineWindow(const EngineWindowProperties &properties)
{
#ifdef LOG_ENABLED
    std::cout << "Initializing window...\n";
#endif // LOG_ENABLED

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(properties.width, properties.height, properties.title.data(), nullptr, nullptr);

#ifdef LOG_ENABLED
    std::cout << "Finshed!\n";
#endif // LOG_ENABLED
}

EngineWindow::~EngineWindow()
{
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
