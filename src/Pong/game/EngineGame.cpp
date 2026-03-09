#include "EngineGame.h"

#include "graphics/renderers/EditorRenderer.h"
#include "scenes/SceneCreator.h"
#include "scenes/SceneDefinitions.h"
#include "scenes/codec/SceneSerializer.h"
#include "scripts/CSharpCompiler.h"

#include <filesystem>
#include <fstream>
#include <optional>

namespace SSGE::Editor
{

EngineGame::EngineGame(EngineWindow *window, std::string dotnetProjectPath, std::string dotnetProjectName,
                       bool debugModeOn)
    : Game(window,
           std::make_unique<EditorRenderer>(this, window, RendererOptions{debugModeOn, std::optional<uint32_t>()}),
           std::move(dotnetProjectPath), std::move(dotnetProjectName)),
      m_currentScenePath(std::nullopt)
{
}

auto EngineGame::isSceneSaved() const -> bool
{
    return m_currentScenePath.has_value();
}

auto EngineGame::loadScene(std::filesystem::path currentScenePath) -> void
{
    m_currentScenePath = std::move(currentScenePath);

    std::ifstream sceneFile(m_currentScenePath->string(), std::ios::binary | std::ios::in);
    SceneDefinition def = SceneSerializer::deserialize(sceneFile);

    updateGameScriptInfo();

    SceneCreator::CreateScene(this, def);
}

auto EngineGame::saveSceneAs(std::filesystem::path currentScenePath) -> void
{
    m_currentScenePath = currentScenePath;
    saveScene();
}

auto EngineGame::saveScene() const -> void
{
    if (!m_currentScenePath)
    {
        return;
    }
    std::ofstream sceneFile(m_currentScenePath->string(), std::ios::binary | std::ios::out | std::ios::trunc);

    SceneSerializer::serialize(sceneFile, SceneDefinition::FromInstance(getCurrentScene()));
}

auto EngineGame::loadHardcodedScene() -> void
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

    SceneCreator::CreateScene(this, SceneDefinition{.name = "New Scene", .gameObjects = {}});
}

auto EngineGame::setup() -> void
{
    setFPSCap(120);
    std::string result = CSharpCompiler::compile(getDotnetProjectPath(), getDotnetProjectName());

    if (!result.empty())
    {
        throw std::runtime_error("Failure to start initial compilation of dotnet scripts, aborting...");
    }

    loadHardcodedScene();
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
