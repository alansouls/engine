#pragma once
#include "UIView.h"
#include "scenes/GameObject.h"

namespace SSGE
{
class SceneExplorerView : public UIView
{
  public:
    explicit SceneExplorerView(Messenger *messenger);

    auto render(uint32_t currentImage) -> void override;

    auto selectedGameObject() const -> GameObject *;

  private:
    // this should only be used to identify the selected game object
    GameObject *m_selectedGameObject;
};
} // namespace SSGE