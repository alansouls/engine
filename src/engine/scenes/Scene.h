#pragma once
#include "scripts/CSharpExecutionEngine.h"

#include <memory>
#include <string>
#include <vector>

class CollisionManager;
class Renderer;
namespace SSGE
{
class GameObject;
class Scene
{
  public:
    Scene(std::string name, CollisionManager *collisionManager,
          CSharpExecutionEngine *executionEngine);
    ~Scene();

    auto addGameObject(const std::shared_ptr<GameObject> &gameObject) -> void;
    auto removeGameObject(const std::shared_ptr<GameObject> &gameObject) -> void;
    auto gameObjects() -> const std::vector<std::shared_ptr<GameObject>> &;

    auto run() -> void;

    [[nodiscard]] auto getName() const -> const std::string &;

    auto onKeyPressed(int key) -> void;
    auto onKeyReleased(int key) -> void;
    auto onKeyDown(int key) -> void;

  private:
    std::string m_name;
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
    std::vector<GameObject *> m_gameObjectsToInit;
    CollisionManager *m_collisionManager;
    CSharpExecutionEngine *m_executionEngine;
};
} // namespace SSGE