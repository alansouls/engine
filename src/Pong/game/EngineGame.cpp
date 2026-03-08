#include "EngineGame.h"

#include "scenes/SceneCreator.h"
#include "scenes/SceneDefinitions.h"
#include "scenes/codec/SceneSerializer.h"
#include "scripts/CSharpCompiler.h"
#include "utils/NativeDialogUtils.h"

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
                                .type = Component::ComponentType::QuadRenderer,
                                .fields = {},
                            },
                            ComponentDefinition{
                                .name = "SSGEDotNet.Sample.RacketComponent",
                                .type = Component::ComponentType::Script,
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
                                .type = Component::ComponentType::QuadCollider,
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
                                .type = Component::ComponentType::QuadRenderer,
                                .fields = {},
                            },
                            ComponentDefinition{
                                .name = "SSGEDotNet.Sample.RacketComponent",
                                .type = Component::ComponentType::Script,
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
                                .type = Component::ComponentType::QuadCollider,
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
                                                .type = Component::ComponentType::CircleRenderer,
                                                .fields = {}},
                            ComponentDefinition{.name = "SSGEDotNet.Sample.BallComponent",
                                                .type = Component::ComponentType::Script,
                                                .fields = {}},
                            ComponentDefinition{
                                .name = "CircleCollider",
                                .type = Component::ComponentType::CircleCollider,
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

    std::fstream file("scene.sgs", std::ios::trunc | std::ios::out | std::ios::binary);

    if (!file.is_open())
        std::cout << "failed to open " << '\n';

    SceneSerializer::serialize(file, mainScene);

    file.close();

    std::optional<std::ifstream> sceneFile = std::nullopt;

    while (!sceneFile)
    {
        sceneFile = NativeDialogUtils::OpenReadFileFromDialog("Scene Files", "sgs");
    }

    SceneDefinition def2 = SceneSerializer::deserialize(sceneFile.value());

    sceneFile.value().close();

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
