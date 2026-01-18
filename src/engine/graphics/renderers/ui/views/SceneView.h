#pragma once

#include "UIView.h"
#include "engine/graphics/renderers/scene/EditorSceneRenderer.h"

class SceneView final : public UIView
{
  public:
    explicit SceneView(const std::string &name, EditorSceneRenderer *sceneRenderer);
    ~SceneView() override = default;
    auto render(uint32_t currentImage) -> void override;

  private:
    EditorSceneRenderer *m_sceneRenderer;
};
