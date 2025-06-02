#pragma once
#include "UIView.h"

namespace SSGE
{
class SceneExplorerView : public UIView
{
  public:
    explicit SceneExplorerView();

    auto render(uint32_t currentImage) -> void override;
private:
    //this should only be used to identify the selected game object
    void* m_selectedGameObject;
};
} // namespace SSGE