#include "Scene.h"

#include "../collisions/CollisionManager.h"
#include "../input/InputManager.h"
#include "../input/InputState.h"
#include "GameObject.h"
#include "scenes/SceneDefinitions.h"

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

    return rawPtr;
}

auto Scene::removeGameObject(GameObject *gameObject) -> void
{
    auto toDeleteIt =
        std::ranges::find(m_gameObjects, gameObject, [](auto &gameObjectRef) { return gameObjectRef.get(); });

    if (toDeleteIt == m_gameObjects.end())
        return;

    m_gameObjects.erase(toDeleteIt);
    std::erase(m_gameObjectsToInit, gameObject);
}

auto Scene::initForRun() -> void
{
    m_collisionManager.clear();
    m_gameObjectsToInit.clear();

    for (auto &gameObject : m_gameObjects)
    {
        m_collisionManager.addGameObjectCollider(gameObject.get());
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
