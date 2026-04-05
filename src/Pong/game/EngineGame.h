#pragma once

#include "../../engine/scenes/Game.h"
#include <filesystem>

namespace SSGE::Editor
{
class EngineGame : public Game
{
  public:
    EngineGame(EngineWindow *window, std::string dotnetProjectPath, std::string dotnetProjectName, bool debugModelOn);

    auto setup() -> void override;
    auto run() -> void override;

    auto isSceneSaved() const -> bool;
    auto loadScene(std::filesystem::path currentScenePath) -> void;
    auto saveSceneAs(std::filesystem::path currentScenePath) -> void;
    auto saveScene() const -> void;

    auto isProjectLoaded() const -> bool;
    auto loadProject(std::filesystem::path projectFilePath) -> void;
    auto createProject(std::string projectName, std::filesystem::path projectPath) -> void;

  protected:
    void preRun() override;

  private:
    bool m_compiling = false;
    bool m_pendingInitialSceneLoad = false;
    std::optional<std::filesystem::path> m_currentProjectPath;
    std::optional<std::filesystem::path> m_currentScenePath;

    // TODO: remove this once we have the sample in git
    auto loadHardcodedScene() -> void;
};
} // namespace SSGE::Editor
