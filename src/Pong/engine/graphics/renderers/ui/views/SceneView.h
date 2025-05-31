#pragma once

#include "UIView.h"
#include "engine/graphics/renderers/scene/SceneRenderer.h"

class SceneView final : public UIView
{
  public:
    explicit SceneView(SceneRenderer *sceneRenderer);
    ~SceneView() override = default;
    auto render(uint32_t currentImage) -> void override;

  private:
    SceneRenderer *m_sceneRenderer;
};
