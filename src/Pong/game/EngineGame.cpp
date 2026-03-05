#include "EngineGame.h"

#include "scenes/SceneCreator.h"
#include "scenes/SceneDefinitions.h"
#include "scenes/codec/SceneSerializer.h"
#include "scripts/CSharpCompiler.h"

#include <fstream>
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
                                .type = ComponentDefinition::ComponentType::QuadRenderer,
                                .fields = {},
                            },
                            ComponentDefinition{
                                .name = "SSGEDotNet.Sample.RacketComponent",
                                .type = ComponentDefinition::ComponentType::Script,
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
                                .type = ComponentDefinition::ComponentType::QuadCollider,
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
                                .type = ComponentDefinition::ComponentType::QuadRenderer,
                                .fields = {},
                            },
                            ComponentDefinition{
                                .name = "SSGEDotNet.Sample.RacketComponent",
                                .type = ComponentDefinition::ComponentType::Script,
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
                                .type = ComponentDefinition::ComponentType::QuadCollider,
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
                            ComponentDefinition{.name = "CircleRenderer",
                                                .type = ComponentDefinition::ComponentType::CircleRenderer,
                                                .fields = {}},
                            ComponentDefinition{.name = "SSGEDotNet.Sample.BallComponent",
                                                .type = ComponentDefinition::ComponentType::Script,
                                                .fields = {}},
                            ComponentDefinition{
                                .name = "CircleCollider",
                                .type = ComponentDefinition::ComponentType::CircleCollider,
                                .fields =
                                    {
                                        ComponentFieldDefinition{
                                            .name = "IsPrimary",
                                            .value = "T",
                                        },
                                    },
                            },
                        },
                },
            },
    };

    std::fstream file("scene.bin", std::ios::trunc | std::ios::out | std::ios::binary);

    if (!file.is_open())
        std::cout << "failed to open " << '\n';

    SceneSerializer::serialize(file, mainScene);

    file.close();

    file.open("scene.bin", std::ios::in | std::ios::binary);

    if (!file.is_open())
        std::cout << "failed to open " << '\n';

    SceneDefinition def2 = SceneSerializer::deserialize(file);

    file.close();

    SceneCreator::CreateScene(this, def2);
}

auto EngineGame::setup() -> void
{
    setFPSCap(120);
    std::string result = CSharpCompiler::compile(getDotnetProjectPath(), getDotnetProjectName());

    if (!result.empty())
    {
        throw std::runtime_error("Failure to start initial compilation of dotnet scripts, aborting...");
    }

    loadScene();
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