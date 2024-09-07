#include "../engine/scenes/Game.h"

class PongGame : public Game
{
public:
	PongGame(bool debugModeOn, GLFWwindow* window);

	void setup() override;

private:
	Renderer* createRenderer(bool debugModeOn, GLFWwindow* window);
};