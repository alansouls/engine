#pragma once
#include "collisions/CollisionManager.h"
#include "scripts/CSharpExecutionEngine.h"

#include <memory>
#include <ranges>
#include <string>
#include <vector>

class CollisionManager;
class EditorRenderer;
namespace SSGE
{
class GameObject;
class InputManager;
class InputState;

class Scene
{
  public:
    Scene(std::string name, CSharpExecutionEngine *executionEngine, InputManager *inputManager);
    ~Scene();

    auto addGameObject(std::unique_ptr<GameObject> gameObject) -> GameObject *;
    auto removeGameObject(GameObject *gameObject) -> void;

    [[nodiscard]] auto gameObjects() const
    {
        return m_gameObjects | std::views::transform([](auto &gameObject) { return gameObject.get(); });
    }

    auto initForRun() -> void;
    auto run() -> void;

    [[nodiscard]] auto getName() const -> const std::string &;

    // Get the input manager for this scene
    [[nodiscard]] auto getInputManager() const -> InputManager *;

    // Get the current input state
    [[nodiscard]] auto getInputState() const -> const InputState &;

  private:
    std::string m_name;
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::vector<GameObject *> m_gameObjectsToInit;
    CollisionManager m_collisionManager;
    CSharpExecutionEngine *m_executionEngine;
    InputManager *m_inputManager;
};
} // namespace SSGE