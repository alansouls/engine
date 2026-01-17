#include "UIRenderer.h"

#include <memory>

#include "../../EngineWindow.h"
#include "backends/imgui_impl_glfw.h"
#include "imgui.h"
#include "views/SceneExplorerView.h"
#include "views/SceneView.h"

#include <format>

using namespace SSGE;

UIRenderer::UIRenderer(EngineWindow *window, VulkanDriver *driver) : m_window(window), m_driver(driver)
{
}

UIRenderer::~UIRenderer()
{
    cleanup();
}

auto UIRenderer::init(EditorSceneRenderer *sceneRenderer) -> void
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
    m_views.push_back(std::make_unique<SceneExplorerView>());
}

auto UIRenderer::renderMenu() const -> void
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", false))
        {
            ImGui::EndMenu();
        } // TODO implmement file menu
        if (ImGui::BeginMenu("View"))
        {
            for (auto &view : m_views)
            {
                if (ImGui::MenuItem(view->getName().data(), nullptr, view->getOpen()))
                {
                    view->setOpen(true);
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

auto UIRenderer::renderUI(uint32_t currentImage) const -> ImDrawData *
{
    VulkanDriver::beginUIFrame();
    EngineWindow::beginUIFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    renderMenu();

    for (auto &view : m_views)
    {
        if (view->getOpen())
        {
            view->render(currentImage);
        }
    }

    ImGui::Render();
    return ImGui::GetDrawData();
}

void UIRenderer::cleanup()
{
    VulkanDriver::cleanupForUI();
    EngineWindow::cleanupForUI();
}
