#include "PongGame.h"
#include "../engine/graphics/renderers/Renderer.h"
#include "../engine/scenes/Scene.h"
#include "components/Ball.h"
#include "components/Racket.h"
#include <optional>

PongGame::PongGame(bool debugModeOn, EngineWindow *window) : Game(window, createRenderer(debugModeOn, window))
{
}

void PongGame::setup()
{
    setFPSCap(120);

    auto mainScene = addScene("main");

    auto leftRacket = std::make_shared<SSGE::GameObject>("Left Racket");
    leftRacket->addComponent<Racket>(leftRacket.get(), true);
    auto rightRacket = std::make_shared<SSGE::GameObject>("Right Racket");
    rightRacket->addComponent<Racket>(rightRacket.get(), false);

    mainScene->addGameObject(leftRacket);
    // mainScene->addGameObject(rightRacket);
    //
    // auto ball = std::make_shared<SSGE::GameObject>("Ball");
    // ball->addComponent<Ball>(ball.get());
    // mainScene->addGameObject(ball);

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
    return new Renderer(window, RendererOptions{debugModeOn, std::optional<uint32_t>()});
}
