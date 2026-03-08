#include "SceneExplorerView.h"

#include "engine/collisions/CircleCollider.h"
#include "engine/collisions/QuadCollider.h"
#include "engine/scenes/Game.h"
#include "engine/scenes/GameObject.h"
#include "engine/scenes/Scene.h"
#include "engine/scenes/components/CircleRendererComponent.h"
#include "engine/scenes/components/QuadRendererComponent.h"
#include "engine/scripts/GameAssemblyInfo.h"
#include "engine/scripts/components/ScriptComponent.h"
#include "imgui_internal.h"

#include <cstring>

namespace SSGE
{
SceneExplorerView::SceneExplorerView(Messenger *messenger)
    : UIView("Scene Explorer", messenger), m_selectedGameObject(nullptr)
{
    m_open = true;
}

auto SceneExplorerView::render(uint32_t currentImage) -> void
{
    static ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    auto game = Game::getInstance();

    if (game == nullptr)
    {
        return;
    }

    auto currentScene = game->getCurrentScene();

    ImGui::Begin("Scene Explorer", &m_open);

    if (currentScene != nullptr)
    {
        if (ImGui::TreeNode(currentScene->getName().data()))
        {
            // Right-click on the scene tree node for scene context menu
            renderSceneContextMenu(currentScene);

            for (GameObject *gameObject : currentScene->gameObjects())
            {
                if (ImGui::Selectable(gameObject->getName().data(), m_selectedGameObject == gameObject,
                                      ImGuiSelectableFlags_SelectOnClick))
                {
                    m_selectedGameObject = m_selectedGameObject == gameObject ? nullptr : gameObject;
                    sendMessage("SelectedGameObjectChanged", m_selectedGameObject);
                }

                // Right-click on a game object for game object context menu
                if (ImGui::BeginPopupContextItem(("go_ctx_" + gameObject->getName()).c_str()))
                {
                    m_contextMenuGameObject = gameObject;
                    renderGameObjectContextMenu(gameObject);
                    ImGui::EndPopup();
                }
            }
            ImGui::TreePop();
        }
    }

    // Popup modal for entering the new game object name
    if (ImGui::BeginPopupModal("Add Game Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter the name for the new Game Object:");
        ImGui::SetNextItemWidth(250.0f);

        bool enterPressed = ImGui::InputText("##NewGameObjectName", m_newGameObjectName, sizeof(m_newGameObjectName),
                                             ImGuiInputTextFlags_EnterReturnsTrue);

        // Auto-focus the input field when the popup opens
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere(-1);
        }

        if (ImGui::Button("Create", ImVec2(120, 0)) || enterPressed)
        {
            if (std::strlen(m_newGameObjectName) > 0)
            {
                auto scene = Game::getInstance()->getCurrentScene();
                if (scene != nullptr)
                {
                    scene->addGameObject(std::make_unique<GameObject>(m_newGameObjectName));
                }
                std::memset(m_newGameObjectName, 0, sizeof(m_newGameObjectName));
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            std::memset(m_newGameObjectName, 0, sizeof(m_newGameObjectName));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

auto SceneExplorerView::renderSceneContextMenu(Scene *scene) -> void
{
    if (ImGui::BeginPopupContextItem("scene_context_menu"))
    {
        if (ImGui::MenuItem("Add Game Object"))
        {
            ImGui::OpenPopup("Add Game Object");
        }
        ImGui::EndPopup();
    }
}

auto SceneExplorerView::renderGameObjectContextMenu(GameObject *gameObject) -> void
{
    if (ImGui::BeginMenu("Add Component"))
    {
        // Renderer components
        if (ImGui::BeginMenu("Renderers"))
        {
            if (ImGui::MenuItem("Quad Renderer"))
            {
                gameObject->addComponent<QuadRendererComponent>(gameObject);
            }
            if (ImGui::MenuItem("Circle Renderer"))
            {
                gameObject->addComponent<CircleRendererComponent>(gameObject);
            }
            ImGui::EndMenu();
        }

        // Collider components
        if (ImGui::BeginMenu("Colliders"))
        {
            if (ImGui::MenuItem("Quad Collider"))
            {
                gameObject->addComponent<QuadCollider>(false, gameObject);
            }
            if (ImGui::MenuItem("Circle Collider"))
            {
                gameObject->addComponent<CircleCollider>(false, gameObject);
            }
            ImGui::EndMenu();
        }

        // Script components — list each C# class as a separate option
        auto game = Game::getInstance();
        if (game != nullptr && game->isGameAssemblyLoaded())
        {
            const auto &assemblyInfo = game->gameAssemblyInfo();
            if (!assemblyInfo.Components.empty())
            {
                if (ImGui::BeginMenu("Scripts"))
                {
                    for (const auto &scriptInfo : assemblyInfo.Components)
                    {
                        if (ImGui::MenuItem(scriptInfo.Name.c_str()))
                        {
                            gameObject->addComponent<ScriptComponent>(gameObject, scriptInfo.FullName);
                        }
                    }
                    ImGui::EndMenu();
                }
            }
        }

        ImGui::EndMenu();
    }
}

auto SceneExplorerView::selectedGameObject() const -> GameObject *
{
    return m_selectedGameObject;
}
} // namespace SSGE
