#pragma once

#include "../../../engine/graphics/EngineWindow.h"
#include "../../../engine/graphics/renderers/Renderer.h"
#include "game/EngineGame.h"
#include "ui/UIRenderer.h"

class EditorSceneRenderer;
class GraphicsDriver;
class RectangleItem;
class RendererItem;

namespace SSGE::Editor
{
class EditorRenderer final : public SSGE::Renderer
{
  public:
    EditorRenderer(SSGE::Editor::EngineGame *game, EngineWindow *mainWindow, const SSGE::RendererOptions &options);
    ~EditorRenderer() override;

    [[nodiscard]] auto getSceneWidth() const -> uint32_t override;
    [[nodiscard]] auto getSceneHeight() const -> uint32_t override;

    auto resetSceneRenderers() -> void;

  protected:
    auto preRender(uint32_t currentFrame) -> void override;
    auto drawFrame(uint32_t currentFrame) -> void override;

  private:
    SSGE::Editor::EngineGame *m_game;
    ImDrawData *m_drawData;
    std::unique_ptr<UIRenderer> m_uiRenderer;
    std::unique_ptr<EditorSceneRenderer> m_editorSceneRenderer;
    std::unique_ptr<EditorSceneRenderer> m_gameSceneRenderer;
};
} // namespace SSGE::Editor
