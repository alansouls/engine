#include "Scene.h"

#include "../collisions/CollisionManager.h"
#include "../input/InputManager.h"
#include "../input/InputState.h"
#include "Game.h"
#include "GameObject.h"

#include <utility>

namespace SSGE
{
Scene::Scene(std::string name, CSharpExecutionEngine *executionEngine, InputManager *inputManager)
    : m_name(std::move(name)), m_executionEngine(executionEngine), m_inputManager(inputManager)
{
}

Scene::~Scene() = default;

void Scene::addGameObject(const std::shared_ptr<GameObject> &gameObject)
{
    m_gameObjects.push_back(gameObject);
    m_gameObjectsToInit.push_back(gameObject.get());
}

void Scene::removeGameObject(const std::shared_ptr<GameObject> &gameObject)
{
    std::erase(m_gameObjects, gameObject);
    std::erase(m_gameObjectsToInit, gameObject.get());
}

auto Scene::gameObjects() -> const std::vector<std::shared_ptr<GameObject>> &
{
    return m_gameObjects;
}

auto Scene::initForRun() -> void
{
    m_collisionManager.clear();
    m_gameObjectsToInit.clear();

    for (auto &gameObject : m_gameObjects)
    {
        m_collisionManager.addGameObjectCollider(gameObject);
        m_gameObjectsToInit.push_back(gameObject.get());
    }
}

void Scene::run()
{
    for (auto gameObject : m_gameObjectsToInit)
    {
        gameObject->init();
    }

    m_gameObjectsToInit.clear();

    m_collisionManager.checkCollisions();

    for (auto &gameObject : m_gameObjects)
    {
        gameObject->update();
    }
}

const std::string &Scene::getName() const
{
    return m_name;
}

void Scene::onKeyPressed(int key)
{
    for (auto &gameObject : m_gameObjects)
    {
        // gameObject->onKeyPressed(key);
    }
}

void Scene::onKeyReleased(int key)
{
    for (auto &gameObject : m_gameObjects)
    {
        // gameObject->onKeyReleased(key);
    }
}

void Scene::onKeyDown(int key)
{
    for (auto &gameObject : m_gameObjects)
    {
        // gameObject->onKeyDown(key);
    }
}

auto Scene::getInputManager() const -> InputManager *
{
    return m_inputManager;
}

auto Scene::getInputState() const -> const InputState &
{
    return m_inputManager->getInputState();
}
} // namespace SSGE
