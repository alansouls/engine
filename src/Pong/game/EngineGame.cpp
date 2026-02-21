#include "EngineGame.h"
#include "../../engine/scenes/Scene.h"
#include "../engine/scripts/components/ScriptComponent.h"
#include "../graphics/renderers/EditorRenderer.h"

#include "../engine/scenes/GameObject.h"
#include "../engine/scenes/components/CircleRendererComponent.h"
#include "../engine/scenes/components/QuadRendererComponent.h"
#include "collisions/CircleCollider.h"
#include "collisions/QuadCollider.h"
#include "scripts/CSharpCompiler.h"

#include <optional>

namespace SSGE::Editor
{

EngineGame::EngineGame(EngineWindow *window, Renderer *renderer, std::string dotnetProjectPath,
                       std::string dotnetProjectName)
    : Game(window, renderer, std::move(dotnetProjectPath), std::move(dotnetProjectName))
{
}

auto EngineGame::setup() -> void
{
    setFPSCap(120);

    auto mainScene = addScene("main");

    auto leftRacket = std::make_shared<GameObject>("Left Racket");
    leftRacket->addComponent<QuadRendererComponent>(leftRacket.get());
    auto &leftRacketScript =
        leftRacket->addComponent<ScriptComponent>(leftRacket.get(), "SSGEDotNet.Sample.RacketComponent");
    leftRacketScript.setManagedProperty<bool>("IsLeft", true);
    auto &collider =
        leftRacket->addComponent<QuadCollider>(false, leftRacket.get(), glm::vec2{0.0f, 0.0f}, 50.0f, 90.0f);
    collider.setLayer("racket");
    auto rightRacket = std::make_shared<GameObject>("Right Racket");
    rightRacket->addComponent<QuadRendererComponent>(rightRacket.get());
    rightRacket->addComponent<ScriptComponent>(rightRacket.get(), "SSGEDotNet.Sample.RacketComponent");
    auto &rightCollider =
        rightRacket->addComponent<QuadCollider>(false, rightRacket.get(), glm::vec2{0.0f, 0.0f}, 50.0f, 90.0f);
    rightCollider.setLayer("racket");

    mainScene->addGameObject(leftRacket);
    mainScene->addGameObject(rightRacket);

    auto ball = std::make_shared<GameObject>("Ball");
    ball->addComponent<CircleCollider>(true, ball.get(), glm::vec2{0.0f, 0.0f}, 15.0f);
    ball->addComponent<CircleRendererComponent>(ball.get());
    ball->addComponent<ScriptComponent>(ball.get(), "SSGEDotNet.Sample.BallComponent");
    mainScene->addGameObject(ball);

    setCurrentScene("main");

    std::string result = CSharpCompiler::compile(getDotnetProjectPath(), getDotnetProjectName());

    if (!result.empty())
    {
        throw std::runtime_error("Failure to start initial compilation of dotnet scripts, aborting...");
    }
}

void EngineGame::run()
{
    Game::run();
}

void EngineGame::preRun()
{
    if (CSharpCompiler::isCompiling() && !m_compiling)
    {
        m_compiling = true;
    }
    else if (!CSharpCompiler::isCompiling() && m_compiling)
    {
        m_compiling = false;
        updateGameScriptInfo();
    }
}

} // namespace SSGE::Editor
