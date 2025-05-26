#include "PongGame.h"
#include "../engine/graphics/renderers/Renderer.h"
#include "../engine/scenes/Scene.h"
#include "gameObjects/Ball.h"
#include "gameObjects/Racket.h"
#include <optional>

PongGame::PongGame(bool debugModeOn, EngineWindow *window) : Game(window, createRenderer(debugModeOn, window))
{
}

void PongGame::setup()
{
    setFPSCap(120);

    auto leftRacket = std::make_shared<Racket>(true);
    auto rightRacket = std::make_shared<Racket>(false);

    auto mainScene = addScene("main");

    mainScene->addGameObject(leftRacket);
    mainScene->addGameObject(rightRacket);

    for (int i = 0; i < 200; ++i)
    {
        mainScene->addGameObject(std::make_shared<Ball>());
    }

    setCurrentScene("main");
}

void PongGame::onKeyPressed(int key)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        isPaused() ? resume() : pause();
    }
}

Renderer *PongGame::createRenderer(bool debugModeOn, EngineWindow *window)
{
    return new Renderer(window,
                        RendererOptions{RendererOptions::RendererType::Vulkan, debugModeOn, std::optional<uint32_t>()});
}
