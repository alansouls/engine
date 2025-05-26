#include "UIRenderer.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "../../EngineWindow.h"

UIRenderer::UIRenderer(EngineWindow *window, VulkanDriver *driver) : m_window(window), m_driver(driver)
{
}

void UIRenderer::init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    m_window->initForUI();
    m_driver->initForUI();
}

void UIRenderer::renderUI()
{
    m_driver->beginUIFrame();
    m_window->beginUIFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    ImGui::ShowDemoWindow();
    ImGui::Render();
    m_driver->endUIFrame(ImGui::GetDrawData());
    m_window->endUIFrame();
}

void UIRenderer::cleanup()
{
    m_driver->cleanupForUI();
    m_window->cleanupForUI();
}
