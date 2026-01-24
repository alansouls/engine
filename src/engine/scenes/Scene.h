#pragma once
#include "collisions/CollisionManager.h"
#include "scripts/CSharpExecutionEngine.h"

#include <memory>
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

    auto addGameObject(const std::shared_ptr<GameObject> &gameObject) -> void;
    auto removeGameObject(const std::shared_ptr<GameObject> &gameObject) -> void;
    auto gameObjects() -> const std::vector<std::shared_ptr<GameObject>> &;

    auto initForRun() -> void;
    auto run() -> void;

    [[nodiscard]] auto getName() const -> const std::string &;

    auto onKeyPressed(int key) -> void;
    auto onKeyReleased(int key) -> void;
    auto onKeyDown(int key) -> void;

    // Get the input manager for this scene
    [[nodiscard]] auto getInputManager() const -> InputManager *;

    // Get the current input state
    [[nodiscard]] auto getInputState() const -> const InputState &;

  private:
    std::string m_name;
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    std::vector<GameObject *> m_gameObjectsToInit;
    CollisionManager m_collisionManager;
    CSharpExecutionEngine *m_executionEngine;
    InputManager *m_inputManager;
};
} // namespace SSGE