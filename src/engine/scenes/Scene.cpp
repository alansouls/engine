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

auto Scene::addGameObject(std::unique_ptr<GameObject> gameObject) -> GameObject *
{
    auto rawPtr = gameObject.get();
    m_gameObjects.emplace_back(std::move(gameObject));
    m_gameObjectsToInit.push_back(rawPtr);
}

auto Scene::removeGameObject(GameObject *gameObject) -> void
{
    auto toDeleteIt = m_gameObjects.end();

    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it)
    {
        if (it->get() == gameObject)
            toDeleteIt = it;
    }

    m_gameObjects.erase(toDeleteIt);
    std::erase(m_gameObjectsToInit, gameObject);
}

auto Scene::gameObjects() const -> std::vector<GameObject *>
{
    std::vector<GameObject *> gameObjects(m_gameObjects.size());
    int i = 0;
    for (auto &gameObject : m_gameObjects)
    {
        gameObjects[i] = gameObject.get();
        ++i;
    }

    return gameObjects;
}

auto Scene::initForRun() -> void
{
    m_collisionManager.clear();
    m_gameObjectsToInit.clear();

    for (auto &gameObject : m_gameObjects)
    {
        m_collisionManager.addGameObjectCollider(gameObject);
        m_gameObjectsToInit.push_back(gameObject.get());
        gameObject->init(); // resets state
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

auto Scene::getInputManager() const -> InputManager *
{
    return m_inputManager;
}

auto Scene::getInputState() const -> const InputState &
{
    return m_inputManager->getInputState();
}
} // namespace SSGE
