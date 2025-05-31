#include "UIRenderer.h"

#include <memory>

#include "../../EngineWindow.h"
#include "backends/imgui_impl_glfw.h"
#include "views/SceneView.h"

UIRenderer::UIRenderer(EngineWindow *window, VulkanDriver *driver) : m_window(window), m_driver(driver)
{
}

auto UIRenderer::init(SceneRenderer *sceneRenderer) -> void
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

    m_views.push_back(std::make_unique<SceneView>(sceneRenderer));
}

auto UIRenderer::renderUI(uint32_t currentImage) const -> ImDrawData *
{
    VulkanDriver::beginUIFrame();
    EngineWindow::beginUIFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    ImGui::ShowDemoWindow();

    for (auto &view : m_views)
    {
        view->render(currentImage);
    }

    ImGui::Render();
    return ImGui::GetDrawData();
}

void UIRenderer::cleanup()
{
    VulkanDriver::cleanupForUI();
    EngineWindow::cleanupForUI();
}
