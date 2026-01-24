#pragma once

#include "../EngineWindow.h"
#include "../drivers/GraphicsOperation.h"
#include "Renderer.h"
#include "ui/UIRenderer.h"

class EditorSceneRenderer;
class GraphicsDriver;
class RectangleItem;
class RendererItem;
class EditorRenderer final : public SSGE::Renderer
{
  public:
    EditorRenderer(EngineWindow *mainWindow, const SSGE::RendererOptions &options);
    ~EditorRenderer() override;

    [[nodiscard]] auto getSceneWidth() const -> uint32_t;
    [[nodiscard]] auto getSceneHeight() const -> uint32_t;

protected:
    auto preRender(uint32_t currentFrame) -> void override;
    auto drawFrame(uint32_t currentFrame) -> void override;

  private:
    ImDrawData *m_drawData;
    std::unique_ptr<UIRenderer> m_uiRenderer;
    std::unique_ptr<EditorSceneRenderer> m_editorSceneRenderer;
    std::unique_ptr<EditorSceneRenderer> m_gameSceneRenderer;
};
