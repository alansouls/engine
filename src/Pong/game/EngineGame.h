#pragma once

#include "../../engine/scenes/Game.h"

class EngineGame : public Game
{
  public:
    EngineGame(EngineWindow *window, SSGE::Renderer *renderer, std::string dotnetProjectPath,
               std::string dotnetProjectName);

    auto setup() -> void override;
    auto run() -> void override;
};