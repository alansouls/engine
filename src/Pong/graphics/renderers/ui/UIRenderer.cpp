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
    : m_messenger(std::make_unique<Messenger>()), m_game(game), m_window(window), m_driver(driver)
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

    if (!m_game->isProjectLoaded())
    {
        ImGui::OpenPopup("Welcome to SSGE");
    }

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

    renderStartupPopup();
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

auto UIRenderer::renderStartupPopup() const -> void
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(500, 360), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));

    if (!ImGui::BeginPopupModal("Welcome to SSGE", nullptr,
                                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
    {
        return;
    }

    if (!m_showNewProjectForm)
    {
        ImGui::Text("Recent Projects");
        ImGui::Separator();

        ImGui::BeginChild("RecentProjects", ImVec2(0, 230), true);
        ImGui::TextDisabled("No recent projects");
        ImGui::EndChild();

        ImGui::Spacing();

        if (ImGui::Button("Load Project", ImVec2(120, 0)))
        {
            auto path = Editor::NativeDialogUtils::GetReadFileFromDialog("Project Files", "sgp");
            if (path)
            {
                m_game->loadProject(*path);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("New Project", ImVec2(120, 0)))
        {
            m_showNewProjectForm = true;
            m_newProjectName[0] = '\0';
            m_newProjectFolderPath.clear();
        }
    }
    else
    {
        ImGui::Text("New Project");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Project Name:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##projectname", m_newProjectName, sizeof(m_newProjectName));

        ImGui::Spacing();
        ImGui::Text("Location:");

        float browseButtonWidth = 80.0f;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - browseButtonWidth - spacing);
        if (m_newProjectFolderPath.empty())
            ImGui::InputText("##projectfolder", const_cast<char *>(""), 1, ImGuiInputTextFlags_ReadOnly);
        else
        {
            char buf[512];
            std::snprintf(buf, sizeof(buf), "%s", m_newProjectFolderPath.c_str());
            ImGui::InputText("##projectfolder", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse", ImVec2(browseButtonWidth, 0)))
        {
            auto folder = Editor::NativeDialogUtils::GetFolderFromDialog();
            if (folder)
            {
                m_newProjectFolderPath = folder->string();
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canCreate = m_newProjectName[0] != '\0' && !m_newProjectFolderPath.empty();
        if (!canCreate)
            ImGui::BeginDisabled();
        if (ImGui::Button("Create", ImVec2(100, 0)))
        {
            m_game->createProject(m_newProjectName, std::filesystem::path(m_newProjectFolderPath));
            m_showNewProjectForm = false;
            ImGui::CloseCurrentPopup();
        }
        if (!canCreate)
            ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Back", ImVec2(100, 0)))
        {
            m_showNewProjectForm = false;
        }
    }

    ImGui::EndPopup();
}
