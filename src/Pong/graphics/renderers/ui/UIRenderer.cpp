#include "UIRenderer.h"
#include "core/Messenger.h"
#include "imgui.h"
#include "scenes/Game.h"
#include "scenes/Scene.h"
#include "scenes/codec/SceneSerializer.h"
#include "scripts/CSharpCompiler.h"
#include "utils/NativeDialogUtils.h"
#include "views/InspectorView.h"
#include "views/SceneExplorerView.h"
#include "views/SceneView.h"
#include <memory>

using namespace SSGE;

UIRenderer::UIRenderer(EngineWindow *window, VulkanDriver *driver) : m_window(window), m_driver(driver)
{
    m_messenger = std::make_unique<Messenger>();
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    m_window->initForUI();
    m_driver->initForUI(2);

    m_views.push_back(std::make_unique<SceneView>("Scene", sceneRenderer, false));
    m_views.push_back(std::make_unique<SceneView>("Game", gameSceneRenderer, true));
    m_views.push_back(std::make_unique<SceneExplorerView>(m_messenger.get()));
    m_views.push_back(std::make_unique<InspectorView>(m_messenger.get()));
}

auto UIRenderer::renderMenu() const -> void
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File", false))
        {
            ImGui::EndMenu();
        } // TODO implmement file menu

        Game *game = Game::getInstance();

        assert(game);

        Scene *currentScene = game->getCurrentScene();

        if (currentScene && ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("Load Scene"))
            {
                // TODO: Add code to load scene from filesystem
            }

            if (ImGui::MenuItem("Save Scene"))
            {
                std::optional<std::ofstream> sceneFile =
                    Editor::NativeDialogUtils::OpenSaveFileFromDialog("Scene Files", "sgs");
                if (sceneFile)
                {
                    SceneSerializer::serialize(sceneFile.value(), SceneDefinition::FromInstance(currentScene));
                }
            }

            ImGui::EndMenu();
        }

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

        if (ImGui::BeginMenu("Game"))
        {

            if (ImGui::MenuItem("Run", "F5", false, !game->isStarted()) && !game->isStarted())
            {
                game->start();
            }

            if (ImGui::MenuItem(game->isPaused() ? "Resume" : "Pause", "F10", false, game->isStarted()) &&
                game->isStarted())
            {
                if (game->isPaused())
                    game->resume();
                else
                    game->pause();
            }

            if (ImGui::MenuItem("Stop", "F6", false, game->isStarted()) && game->isStarted())
            {
                game->stop();
            }

            if (ImGui::MenuItem("Rebuild Scripts", "CTRL + B", false, !game->isStarted()) && !game->isStarted())
            {
                CSharpCompiler::startCompile(game->getDotnetProjectPath(), game->getDotnetProjectName());
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

    if (CSharpCompiler::isCompiling())
    {
        const char *popupTitle = "Building Game Scripts";
        ImGui::OpenPopup(popupTitle);

        ImGui::SetNextWindowSize(ImVec2(200, 60));

        if (ImGui::BeginPopupModal(popupTitle, nullptr, ImGuiWindowFlags_NoResize))
        {
            ImGui::ProgressBar(-1.0f);
            ImGui::EndPopup();
        }
    }

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
