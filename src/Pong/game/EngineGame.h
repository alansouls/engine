#include "../../engine/scenes/Game.h"

class EngineGame : public Game
{
  public:
    EngineGame(bool debugModeOn, EngineWindow *window);

    void setup() override;

    void onKeyPressed(int key) override;
};