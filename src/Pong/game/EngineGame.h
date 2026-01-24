#pragma once

#include "../../engine/scenes/Game.h"

class EngineGame : public Game
{
  public:
    EngineGame(EngineWindow *window, SSGE::Renderer *renderer);

    void setup() override;

    void onKeyPressed(int key) override;
};