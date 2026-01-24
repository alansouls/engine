#pragma once
#include "game/EngineGame.h"
#include "graphics/EngineWindow.h"
#include "graphics/renderers/EditorRenderer.h"

class SSGEEditor
{
  public:
    auto run(bool debugModeOn) -> void;

  private:
    std::unique_ptr<EngineWindow> m_window;
    std::unique_ptr<EditorRenderer> m_renderer;
    std::unique_ptr<EngineGame> m_game;

    auto mainLoop() const -> void;
};
