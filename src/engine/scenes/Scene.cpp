#include "Scene.h"

#include "../collisions/CollisionManager.h"
#include "../graphics/renderers/Renderer.h"
#include "Game.h"
#include "GameObject.h"
#include "engine/scripts/components/ScriptComponent.h"

#include <utility>

namespace SSGE
{
Scene::Scene(std::string name, Renderer *renderer, CollisionManager *collisionManager,
             CSharpExecutionEngine *executionEngine)
    : m_name(std::move(name)), m_renderer(renderer), m_collisionManager(collisionManager),
      m_executionEngine(executionEngine)
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

void Scene::run()
{
    for (auto gameObject : m_gameObjectsToInit)
    {
        gameObject->init();
    }

    m_gameObjectsToInit.clear();

    m_collisionManager->checkCollisions();

    for (auto &gameObject : m_gameObjects)
    {
        gameObject->update();
    }

    m_renderer->render();
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
} // namespace SSGE
