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
    void ShowAddGameObjectPopup(SSGE::Scene *currentScene);

    auto render(uint32_t) -> void override;

    [[nodiscard]] auto selectedGameObject() const -> GameObject *;

  private:
    typedef uint32_t PopupTypes;
    enum PopupType : uint32_t
    {
        PopupType_None = 0,
        PopupType_AddGameObject = 1 << 0
    };
    // this should only be used to identify the selected game object
    GameObject *m_selectedGameObject;

    // Context menu state
    PopupTypes m_popups;
    std::array<char, 128> m_newGameObjectName = {};
    GameObject *m_contextMenuGameObject = nullptr;

    auto renderSceneContextMenu() -> void;
    static auto renderGameObjectContextMenu(GameObject *gameObject) -> void;
};
} // namespace SSGE