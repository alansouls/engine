#include "PongGame.h"
#include "../engine/graphics/renderers/Renderer.h"
#include "gameObjects/Racket.h"
#include "gameObjects/Ball.h"
#include <optional>
#include "../engine/scenes/Scene.h"

PongGame::PongGame(bool debugModeOn, GLFWwindow* window) : Game(window, createRenderer(debugModeOn, window))
{
}

void PongGame::setup()
{
	auto leftRacket = std::make_shared<Racket>(true);
	auto rightRacket = std::make_shared<Racket>(false);

	auto mainScene = addScene("main");

	mainScene->addGameObject(leftRacket);
	mainScene->addGameObject(rightRacket);

	/*for (int i = 0; i < 1000; ++i) {
		auto ball = std::make_shared<Ball>();
		mainScene->addGameObject(ball);
	}*/

	setCurrentScene("main");
}

void PongGame::onKeyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE) {
		isPaused() ? resume() : pause();
	}
}

Renderer* PongGame::createRenderer(bool debugModeOn, GLFWwindow* window)
{
	return new Renderer(window, RendererOptions{ RendererOptions::RendererType::Vulkan, debugModeOn, std::optional<uint32_t>() });
}
