#include "GameObject.h"

#include "Game.h"

namespace SSGE
{
GameObject::GameObject(std::string name, const std::optional<GameObject *> &parent)
    : Component("GameObject", this), m_parent(parent), m_name(std::move(name))
{
}

auto GameObject::init() -> void
{
    for (const auto &component : m_components | std::views::values)
    {
        component->init();
    }
}

auto GameObject::update() -> void
{
    for (const auto &component : m_components | std::views::values)
    {
        component->update();
    }
}

auto GameObject::getName() -> const std::string &
{
    return m_name;
}

auto GameObject::getConstTransform() const -> const Transform &
{
    return m_transform;
}

auto GameObject::getTransform() -> Transform &
{
    return m_transform;
}

GameProperties GameObject::getGameProperties()
{
    return Game::getInstance()->getProperties();
}
} // namespace SSGE