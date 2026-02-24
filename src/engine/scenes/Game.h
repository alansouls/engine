#pragma once

#include "EngineAPI.h"
#include "GameProperties.h"
#include "graphics/EngineWindow.h"
#include "graphics/renderers/Renderer.h"
#include "scripts/CSharpExecutionEngine.h"

#include <GLFW/glfw3.h>
#include <string>

namespace SSGE
{
class Scene;
class InputManager;
} // namespace SSGE
class CollisionManager;
class Game
{
  public:
    Game(EngineWindow *window, SSGE::Renderer *renderer, std::string dotnetProjectPath, std::string dotnetProjectName);
    virtual ~Game() = 0;

    virtual void setup() = 0;

    virtual void run();

    static Game *getInstance();
    static void setInstance(Game *instance);

    SSGE::Scene *addScene(const std::string &name);
    void removeScene(const std::string &name);
    void setCurrentScene(const std::string &name);

    [[nodiscard]] auto getCurrentScene() const -> SSGE::Scene *;

    [[nodiscard]] auto getProperties() const -> GameProperties;

    void pause();
    void resume();

    [[nodiscard]] auto isPaused() const -> bool;

    auto setFPSCap(const std::optional<uint16_t> &fpsCap) -> void;
    [[nodiscard]] auto getFPSCap() const -> const std::optional<uint16_t> &;

    [[nodiscard]] auto getRenderer() const -> SSGE::Renderer &;

    [[nodiscard]] auto getInputManager() const -> SSGE::InputManager *;

    auto start() -> void;
    auto stop() -> void;

    [[nodiscard]] auto isStarted() const -> bool;

    [[nodiscard]] auto getDotnetProjectPath() const -> const std::string &;
    [[nodiscard]] auto getDotnetProjectName() const -> const std::string &;

    auto updateGameScriptInfo() -> void;

    [[nodiscard]] auto isGameAssemblyLoaded() const -> bool;

protected:
    virtual void preRun()
    {
    }

  private:
    std::string m_dotnetProjectPath;
    std::string m_dotnetProjectName;
    SSGE::Renderer *m_renderer;

    std::vector<SSGE::Scene *> m_scenes;
    SSGE::Scene *m_currentScene;

    EngineWindow *m_window;

    bool m_paused;
    bool m_started;
    bool m_shouldRun;
    bool m_gameAssemblyLoaded = false;

    static Game *m_instance;

    std::optional<uint16_t> m_fpsCap;

    std::chrono::nanoseconds m_deltaTime{};

    SSGE::CSharpExecutionEngine *m_scriptExecutionEngine;

    SSGE::InputManager *m_inputManager;

    auto initForRun() -> void;
    auto keyCallback(int key, int scancode, int action, int mods) const -> void;
    auto mouseButtonCallback(int button, int action, int mods) const -> void;
    auto cursorPositionCallback(double xpos, double ypos) const -> void;
    auto scrollCallback(double xoffset, double yoffset) const -> void;
};

// C-style API for interop with C#
extern "C"
{
    SSGE_API auto Game_GetInstance() -> Game *;

    SSGE_API auto Game_GetProperties(Game *game) -> GameProperties;
}
