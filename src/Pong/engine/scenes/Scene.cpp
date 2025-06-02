#include "Scene.h"
#include "Game.h"
#include "GameObject.h"
#include "../graphics/renderers/Renderer.h"
#include "../collisions/CollisionManager.h"

Scene::Scene(const std::string &name, Renderer *renderer, CollisionManager* collisionManager) :
	m_name(name), m_gameObjects(), m_renderer(renderer), m_collisionManager(collisionManager)
{
}

Scene::~Scene()
{
}

void Scene::addGameObject(const std::shared_ptr<GameObject> &gameObject)
{
    m_gameObjects.push_back(gameObject);
    gameObject->init();

    if (gameObject->getRendererItem())
        m_renderer->addItem(gameObject->getRendererItem());
	if (gameObject->getCollider())
		m_collisionManager->addGameObjectCollider(gameObject);
}

void Scene::removeGameObject(const std::shared_ptr<GameObject> &gameObject)
{
    auto iter = m_gameObjects.begin();
    while (iter != m_gameObjects.end())
    {
        if (*iter == gameObject)
        {
            m_gameObjects.erase(iter);
            if (gameObject->getCollider())
                m_collisionManager->removeGameObjectCollider(gameObject);
            break;
        }
        iter++;
    }
}

auto Scene::gameObjects() -> const std::vector<std::shared_ptr<GameObject>>&
{
    return m_gameObjects;
}

void Scene::run()
{
	m_collisionManager->checkCollisions();
    for (auto gameObject : m_gameObjects)
    {
        gameObject->update();
    }

    m_renderer->render();
}

const std::string& Scene::getName() const
{
    return m_name;
}

void Scene::onKeyPressed(int key)
{
	for (auto gameObject : m_gameObjects)
	{
		gameObject->onKeyPressed(key);
	}
}

void Scene::onKeyReleased(int key)
{
	for (auto gameObject : m_gameObjects)
	{
		gameObject->onKeyReleased(key);
	}
}

void Scene::onKeyDown(int key)
{
	for (auto gameObject : m_gameObjects)
	{
		gameObject->onKeyDown(key);
	}
}
