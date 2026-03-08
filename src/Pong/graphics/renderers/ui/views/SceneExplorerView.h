#pragma once
#include "UIView.h"
#include "scenes/GameObject.h"

#include <string>

namespace SSGE
{
class Scene;

class SceneExplorerView : public UIView
{
  public:
    explicit SceneExplorerView(Messenger *messenger);

    auto render(uint32_t currentImage) -> void override;

    auto selectedGameObject() const -> GameObject *;

  private:
    // this should only be used to identify the selected game object
    GameObject *m_selectedGameObject;

    // Context menu state
    char m_newGameObjectName[128] = {};
    GameObject *m_contextMenuGameObject = nullptr;

    auto renderSceneContextMenu(Scene *scene) -> void;
    auto renderGameObjectContextMenu(GameObject *gameObject) -> void;
};
} // namespace SSGE