#include "UIRenderer.h"
#include "imgui.h"
#include "views/SceneExplorerView.h"
#include "views/SceneView.h"
#include "scenes/Game.h"
#include "scripts/CSharpCompiler.h"
#include "views/InspectorView.h"
#include <memory>

using namespace SSGE;
using namespace SSGEEditor;

UIRenderer::UIRenderer(EngineWindow* window, VulkanDriver* driver) : m_window(window), m_driver(driver)
{
    m_messenger = std::make_unique<UIMessenger>();
}

UIRenderer::~UIRenderer()
{
    m_driver->cleanupForUI();
    EngineWindow::cleanupForUI();
}

auto UIRenderer::init(EditorSceneRenderer* sceneRenderer, EditorSceneRenderer* gameSceneRenderer) -> void
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    m_window->initForUI();
    m_driver->initForUI(2);

    m_views.push_back(std::make_unique<SceneView>("Scene", sceneRenderer));
    m_views.push_back(std::make_unique<SceneView>("Game", gameSceneRenderer));
    m_views.push_back(std::make_unique<SceneExplorerView>(m_messenger.get()));
    m_views.push_back(std::make_unique<InspectorView>(m_messenger.get()));
}

auto UIRenderer::renderMenu() const -> void
{
    ImGui::ShowDemoWindow();
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", false))
        {
            ImGui::EndMenu();
        } // TODO implmement file menu

        if (ImGui::BeginMenu("View"))
        {
            for (auto& view : m_views)
            {
                if (ImGui::MenuItem(view->getName().data(), nullptr, view->getOpen()))
                {
                    view->setOpen(true);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Game"))
        {
            Game* game = Game::getInstance();

            if (ImGui::MenuItem("Run", "F5") && !game->isStarted())
            {
                std::string result = SSGEEditor::CSharpCompiler::compile(Game::DotnetProjectPath, "SSGEDotNet.Sample");

                if (result.empty())
                    game->start();
            }

            if (ImGui::MenuItem("Pause", "F6"))
            {
                if (game->isPaused())
                    game->resume();
                else
                    game->pause();
            }

            if (ImGui::MenuItem("Stop", "F7") && game->isStarted())
            {
                game->stop();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

auto UIRenderer::renderUI(uint32_t currentImage) const -> ImDrawData*
{
    VulkanDriver::beginUIFrame();
    EngineWindow::beginUIFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    renderMenu();

    for (auto& view : m_views)
    {
        if (view->getOpen())
        {
            view->render(currentImage);
        }
    }

    ImGui::Render();
    return ImGui::GetDrawData();
}
