#include "UIRenderer.h"
#include "core/Messenger.h"
#include "game/EngineGame.h"
#include "imgui.h"
#include "scenes/Scene.h"
#include "scripts/CSharpCompiler.h"
#include "utils/NativeDialogUtils.h"
#include "views/InspectorView.h"
#include "views/SceneExplorerView.h"
#include "views/SceneView.h"
#include <filesystem>
#include <memory>

using namespace SSGE;
using namespace SSGE::Editor;

UIRenderer::UIRenderer(SSGE::Editor::EngineGame *game, EngineWindow *window, VulkanDriver *driver)
    : m_game(game), m_window(window), m_driver(driver)
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

        Scene *currentScene = m_game->getCurrentScene();

        if (currentScene && ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("Load Scene"))
            {
                std::optional<std::filesystem::path> sceneFile =
                    Editor::NativeDialogUtils::GetReadFileFromDialog("Scene Files", "sgs");

                if (sceneFile)
                {
                    m_game->loadScene(sceneFile.value());
                }
            }

            if (ImGui::MenuItem("Save Scene As"))
            {
                std::optional<std::filesystem::path> sceneFile =
                    Editor::NativeDialogUtils::GetSaveFileFromDialog("Scene Files", "sgs");

                if (sceneFile)
                {
                    m_game->saveSceneAs(sceneFile.value());
                }
            }

            if (ImGui::MenuItem("Save Scene", nullptr, false, m_game->isSceneSaved()))
            {
                m_game->saveScene();
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

            if (ImGui::MenuItem("Run", "F5", false, !m_game->isStarted()) && !m_game->isStarted())
            {
                m_game->start();
            }

            if (ImGui::MenuItem(m_game->isPaused() ? "Resume" : "Pause", "F10", false, m_game->isStarted()) &&
                m_game->isStarted())
            {
                if (m_game->isPaused())
                    m_game->resume();
                else
                    m_game->pause();
            }

            if (ImGui::MenuItem("Stop", "F6", false, m_game->isStarted()) && m_game->isStarted())
            {
                m_game->stop();
            }

            if (ImGui::MenuItem("Rebuild Scripts", "CTRL + B", false, !m_game->isStarted()) && !m_game->isStarted())
            {
                CSharpCompiler::startCompile(m_game->getDotnetProjectPath(), m_game->getDotnetProjectName());
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
