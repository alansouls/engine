#pragma once
#include "../graphics/renderers/Renderer.h"
#include "EngineAPI.h"
#include "GameProperties.h"
#include "scripts/CSharpExecutionEngine.h"

#include <GLFW/glfw3.h>
#include <string>

namespace SSGE
{
class Scene;
class InputManager;
}
class CollisionManager;
class Game
{
  public:
    Game(EngineWindow *window, const RendererOptions &options);
    virtual ~Game() = 0;

    virtual void setup() = 0;

    void run();

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

    [[nodiscard]] auto getRenderer() -> Renderer &;

    [[nodiscard]] auto getInputManager() -> SSGE::InputManager *;

  protected:
    virtual void onKeyPressed(int key);
    virtual void onKeyReleased(int key);
    virtual void onKeyDown(int key);

  private:
    Renderer *m_renderer;

    std::vector<SSGE::Scene *> m_scenes;
    SSGE::Scene *m_currentScene;

    EngineWindow *m_window;

    bool m_paused;

    static Game *m_instance;

    // GLFW callback handlers that forward to InputManager
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    std::optional<uint16_t> m_fpsCap;

    std::chrono::nanoseconds m_deltaTime{};

    SSGE::CSharpExecutionEngine* m_scriptExecutionEngine;

    SSGE::InputManager* m_inputManager;

    const std::string m_gameIdentifier = "Sample";
    //TODO: make this configurable
#ifdef WINDOWS
    const std::string m_dotnetProjectPath = "C:/Users/maiaa/Documents/Dev/personal/engine/src/dotnet/SSGEDotNet";
#else
    #ifdef  LINUX
        const std::string m_dotnetProjectPath = "/mnt/c/Users/maiaa/Documents/Dev/personal/engine/src/dotnet/SSGEDotNet";
    #else
        const std::string m_dotnetProjectPath = "/Users/maia/dev/personal/engine/src/dotnet/SSGEDotNet";
    #endif
#endif
};

// C-style API for interop with C#
extern "C"
{
    SSGE_API auto Game_GetInstance() -> Game*;

    SSGE_API auto Game_GetProperties(Game* game) -> GameProperties;
}
