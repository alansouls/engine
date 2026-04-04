#pragma once

#include "../../engine/scenes/Game.h"
#include "core/Messenger.h"
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

  protected:
    void preRun() override;

  private:
    bool m_compiling = false;
    std::optional<std::filesystem::path> m_currentScenePath;

    // TODO: remove this once we have the sample in git
    auto loadHardcodedScene() -> void;
};
} // namespace SSGE::Editor
