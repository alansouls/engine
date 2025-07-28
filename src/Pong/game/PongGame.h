#include "../../engine/scenes/Game.h"

class PongGame : public Game
{
  public:
    PongGame(bool debugModeOn, EngineWindow *window);

    void setup() override;

    void onKeyPressed(int key) override;

  private:
    static Renderer *createRenderer(bool debugModeOn, EngineWindow *window);
};