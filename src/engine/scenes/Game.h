#pragma once
#include "../graphics/renderers/Renderer.h"
#include "GameProperties.h"
#include <GLFW/glfw3.h>
#include <string>

namespace SSGE
{
class Scene;
}
class CollisionManager;
class Game
{
  public:
    Game(EngineWindow *window, Renderer *renderer);
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

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    CollisionManager *m_collisionManager;

    std::optional<uint16_t> m_fpsCap;

    std::chrono::nanoseconds m_deltaTime{};
};
