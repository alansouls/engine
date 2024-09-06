#include "Scene.h"

Scene::Scene(const std::string &name, Renderer *renderer) :
    m_name(name), m_gameObjects(), m_renderer(renderer)
{
}

Scene::~Scene()
{
    for (auto gameObject : m_gameObjects)
        delete gameObject;
}

void Scene::addGameObject(GameObject *gameObject)
{
    m_gameObjects.push_back(gameObject);
    gameObject->init();

    if (gameObject->getRendererItem())
        m_renderer->addItem(gameObject->getRendererItem());
}

void Scene::removeGameObject(GameObject *gameObject)
{
    auto iter = m_gameObjects.begin();
    while (iter != m_gameObjects.end()) {
        if (*iter == gameObject) {
            m_gameObjects.erase(iter);
            //TODO: remove game object
            break;
        }
        iter++;
    }
}

void Scene::run()
{
    for (auto gameObject : m_gameObjects)
    {
        gameObject->update();
    }

    m_renderer->render();
}
