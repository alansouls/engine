#include "GameObject.h"
#include "Game.h"

GameObject::GameObject() : m_rendererItem(nullptr), m_collider(nullptr)
{
}

GameObject::~GameObject()
{
    if (m_rendererItem != nullptr)
        delete m_rendererItem;
}

RendererItem *GameObject::getRendererItem() const
{
    return m_rendererItem;
}

void GameObject::onKeyPressed(int key)
{
}

void GameObject::onKeyReleased(int key)
{
}

void GameObject::onKeyDown(int key)
{
}

void GameObject::onCollisionEnter(const CollisionInfo& info)
{
}

void GameObject::onCollisionExit(const CollisionInfo& info)
{
}

Collider* GameObject::getCollider() const
{
    return m_collider;
}

void GameObject::setRendererItem(RendererItem* rendererItem)
{
	if (m_rendererItem != nullptr)
		delete m_rendererItem;

    m_rendererItem = rendererItem;
}

GameProperties GameObject::getGameProperties()
{
    return Game::getInstance()->getProperties();
}

void GameObject::setCollider(Collider* collider)
{
	m_collider = collider;
}
