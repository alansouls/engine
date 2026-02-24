#pragma once

#include "UIView.h"

class EditorSceneRenderer;
class SceneView final : public UIView
{
  public:
    explicit SceneView(const std::string &name, EditorSceneRenderer *sceneRenderer, bool forwardGameInput);
    ~SceneView() override = default;
    auto render(uint32_t currentImage) -> void override;

  private:
    EditorSceneRenderer *m_sceneRenderer;
    bool m_forwardGameInput;
};
