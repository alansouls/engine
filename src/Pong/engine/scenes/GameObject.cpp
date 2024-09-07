#include "GameObject.h"
#include "Game.h"

GameObject::GameObject() : m_rendererItem(nullptr)
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
