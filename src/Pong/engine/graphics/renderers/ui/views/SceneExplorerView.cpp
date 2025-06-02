#include "SceneExplorerView.h"

#include "engine/scenes/Game.h"
#include "engine/scenes/GameObject.h"
#include "engine/scenes/Scene.h"
#include "imgui_internal.h"

namespace SSGE
{

SceneExplorerView::SceneExplorerView() : UIView("Scene Explorer"), m_selectedGameObject(nullptr)
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
            for (auto &gameObject : currentScene->gameObjects())
            {
                if (void *gameObjectPtr = gameObject.get();
                    ImGui::Selectable(gameObject->getName().data(), m_selectedGameObject == gameObjectPtr,
                                      ImGuiSelectableFlags_SelectOnClick))
                {
                    m_selectedGameObject = m_selectedGameObject == gameObjectPtr ? nullptr : gameObjectPtr;
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

} // namespace SSGE