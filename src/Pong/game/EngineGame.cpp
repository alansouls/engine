#include "../../engine/graphics/renderers/Renderer.h"
#include "../../engine/scenes/Scene.h"
#include "../engine/scripts/components/ScriptComponent.h"
#include "EngineGame.h"
#include "components/Ball.h"
#include "components/Racket.h"
#include <optional>

EngineGame::EngineGame(bool debugModeOn, EngineWindow *window) 
    : Game(window, RendererOptions{debugModeOn, std::optional<uint32_t>()})
{
}

void EngineGame::setup()
{
    setFPSCap(120);

    auto mainScene = addScene("main");

    auto leftRacket = std::make_shared<SSGE::GameObject>("Left Racket");
    leftRacket->addComponent<Racket>(leftRacket.get(), true);
    auto rightRacket = std::make_shared<SSGE::GameObject>("Right Racket");
    rightRacket->addComponent<Racket>(rightRacket.get(), false);

    mainScene->addGameObject(leftRacket);
    mainScene->addGameObject(rightRacket);

    auto ball = std::make_shared<SSGE::GameObject>("Ball");
    ball->addComponent<Ball>(ball.get());
    ball->addComponent<SSGE::ScriptComponent>(ball.get(), "SSGEDotNet.Sample.TestComponent");
    mainScene->addGameObject(ball);

    setCurrentScene("main");
}

void EngineGame::onKeyPressed(int key)
{
    std::cout << "Key Pressed: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE)
    {
        isPaused() ? resume() : pause();
    }
}