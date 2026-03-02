#include "EngineGame.h"

#include "scenes/SceneCreator.h"
#include "scenes/SceneDefinitions.h"
#include "scripts/CSharpCompiler.h"

#include <optional>
#include <thread>

namespace SSGE::Editor
{

EngineGame::EngineGame(EngineWindow *window, Renderer *renderer, std::string dotnetProjectPath,
                       std::string dotnetProjectName)
    : Game(window, renderer, std::move(dotnetProjectPath), std::move(dotnetProjectName))
{
}

auto EngineGame::loadScene() -> void
{
    updateGameScriptInfo();

    SceneDefinition mainScene = {
        .name = "main",
        .gameObjects =
        {
            GameObjectDefinition{
                .name = "Left Racket",
                .transform{},
                .components =
                {
                    ComponentDefinition{
                        .name = "QuadRenderer",
                        .type = ComponentDefinition::QuadRenderer,
                        .fields = {},
                    },
                    ComponentDefinition{
                        .name = "SSGEDotNet.Sample.RacketComponent",
                        .type = ComponentDefinition::Script,
                        .fields =
                        {
                            ComponentFieldDefinition{
                                .name = "IsLeft",
                                .value = "T",
                            },
                        },
                    },
                    ComponentDefinition{
                        .name = "QuadCollider",
                        .type = ComponentDefinition::QuadCollider,
                        .fields =
                        {
                            ComponentFieldDefinition{
                                .name = "IsPrimary",
                                .value = "",
                            },
                        },
                    },
                },
            },
            GameObjectDefinition{
                .name = "Right Racket",
                .transform{},
                .components =
                {
                    ComponentDefinition{
                        .name = "QuadRenderer",
                        .type = ComponentDefinition::QuadRenderer,
                        .fields = {},
                    },
                    ComponentDefinition{
                        .name = "SSGEDotNet.Sample.RacketComponent",
                        .type = ComponentDefinition::Script,
                        .fields =
                        {
                            ComponentFieldDefinition{
                                .name = "IsLeft",
                                .value = "",
                            },
                        },
                    },
                    ComponentDefinition{
                        .name = "QuadCollider",
                        .type = ComponentDefinition::QuadCollider,
                        .fields =
                        {
                            ComponentFieldDefinition{
                                .name = "IsPrimary",
                                .value = "",
                            },
                        },
                    },
                },
            },
            GameObjectDefinition{
                .name = "Ball",
                .transform{},
                .components =
                {
                    ComponentDefinition{
                        .name = "CircleRenderer", .type = ComponentDefinition::CircleRenderer, .fields = {}},
                    ComponentDefinition{.name = "SSGEDotNet.Sample.BallComponent",
                                        .type = ComponentDefinition::Script,
                                        .fields = {}},
                    ComponentDefinition{
                        .name = "CircleCollider",
                        .type = ComponentDefinition::CircleCollider,
                        .fields =
                        {
                            ComponentFieldDefinition{
                                .name = "IsPrimary",
                                .value = "true",
                            },
                        },
                    },
                },
            },
        },
    };

    SceneCreator::CreateScene(this, mainScene);
}

auto EngineGame::setup() -> void
{
    std::string result = CSharpCompiler::compile(getDotnetProjectPath(), getDotnetProjectName(), [this] {
        loadScene();
    });

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