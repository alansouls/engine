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
	auto leftRacket = new Racket(true);
	auto rightRacket = new Racket(false);
	auto ball = new Ball();

	auto mainScene = addScene("main");

	mainScene->addGameObject(leftRacket);
	mainScene->addGameObject(rightRacket);
	mainScene->addGameObject(ball);

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
