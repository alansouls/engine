#pragma once

#include "../../engine/scenes/Game.h"

namespace SSGE::Editor
{
class EngineGame : public Game
{
  public:
    EngineGame(EngineWindow *window, SSGE::Renderer *renderer, std::string dotnetProjectPath,
               std::string dotnetProjectName);

    auto setup() -> void override;
    auto run() -> void override;

protected:
    void preRun() override;

  private:
    bool m_compiling = false;
};
} // namespace SSGE::Editor
