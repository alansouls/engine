#include "UIRenderer.h"

#include <memory>

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
    m_driver->cleanupForUI();
    EngineWindow::cleanupForUI();
}

auto UIRenderer::init(EditorSceneRenderer *sceneRenderer, EditorSceneRenderer *gameSceneRenderer) -> void
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
    m_driver->initForUI(2);

    m_views.push_back(std::make_unique<SceneView>("Scene", sceneRenderer));
    m_views.push_back(std::make_unique<SceneView>("Game", gameSceneRenderer));
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

    ImGui::ShowDemoWindow();

    ImGui::Render();
    return ImGui::GetDrawData();
}
