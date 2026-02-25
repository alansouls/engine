#include "SceneExplorerView.h"

#include "engine/scenes/Game.h"
#include "engine/scenes/GameObject.h"
#include "engine/scenes/Scene.h"
#include "imgui_internal.h"

namespace SSGE
{
SceneExplorerView::SceneExplorerView(UIMessenger *messenger)
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
            for (GameObject *gameObject : currentScene->gameObjects())
            {
                if (ImGui::Selectable(gameObject->getName().data(), m_selectedGameObject == gameObject,
                                      ImGuiSelectableFlags_SelectOnClick))
                {
                    m_selectedGameObject = m_selectedGameObject == gameObject ? nullptr : gameObject;
                    sendMessage("SelectedGameObjectChanged", m_selectedGameObject);
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

auto SceneExplorerView::selectedGameObject() const -> GameObject *
{
    return m_selectedGameObject;
}
} // namespace SSGE
