#include "EngineGame.h"
#include "../../engine/scenes/Scene.h"
#include "../engine/scripts/components/ScriptComponent.h"
#include "../graphics/renderers/EditorRenderer.h"

#include "../engine/scenes/GameObject.h"
#include "../engine/scenes/components/CircleRendererComponent.h"
#include "../engine/scenes/components/QuadRendererComponent.h"
#include "collisions/CircleCollider.h"
#include "collisions/QuadCollider.h"
#include <optional>

EngineGame::EngineGame(EngineWindow *window, SSGE::Renderer *renderer) : Game(window, renderer)
{
}

void EngineGame::setup()
{
    setFPSCap(120);

    auto mainScene = addScene("main");

    auto leftRacket = std::make_shared<SSGE::GameObject>("Left Racket");
    leftRacket->addComponent<SSGE::QuadRendererComponent>(leftRacket.get());
    auto &leftRacketScript =
        leftRacket->addComponent<SSGE::ScriptComponent>(leftRacket.get(), "SSGEDotNet.Sample.RacketComponent");
    leftRacketScript.setProperty("IsLeft", "true");
    auto &collider =
        leftRacket->addComponent<SSGE::QuadCollider>(false, leftRacket.get(), glm::vec2{0.0f, 0.0f}, 50.0f, 90.0f);
    collider.setLayer("racket");
    auto rightRacket = std::make_shared<SSGE::GameObject>("Right Racket");
    rightRacket->addComponent<SSGE::QuadRendererComponent>(rightRacket.get());
    auto &rightRacketScript =
        rightRacket->addComponent<SSGE::ScriptComponent>(rightRacket.get(), "SSGEDotNet.Sample.RacketComponent");
    rightRacketScript.setProperty("IsLeft", "false");
    auto &rightCollider =
        rightRacket->addComponent<SSGE::QuadCollider>(false, rightRacket.get(), glm::vec2{0.0f, 0.0f}, 50.0f, 90.0f);
    rightCollider.setLayer("racket");

    mainScene->addGameObject(leftRacket);
    mainScene->addGameObject(rightRacket);

    auto ball = std::make_shared<SSGE::GameObject>("Ball");
    ball->addComponent<SSGE::CircleCollider>(true, ball.get(), glm::vec2{0.0f, 0.0f}, 15.0f);
    ball->addComponent<SSGE::CircleRendererComponent>(ball.get());
    ball->addComponent<SSGE::ScriptComponent>(ball.get(), "SSGEDotNet.Sample.BallComponent");
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