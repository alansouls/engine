#include "GameObject.h"

#include "Game.h"

#include <__ranges/views.h>
#include <ranges>
#include <utility>

namespace SSGE
{
GameObject::GameObject(std::string name, const std::optional<GameObject &> &parent)
    : Component("GameObject"), m_parent(parent), m_name(std::move(name))
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

auto GameObject::gameObject() -> GameObject &
{
    return *this;
}

GameProperties GameObject::getGameProperties()
{
    return Game::getInstance()->getProperties();
}

template <class TComponent> auto GameObject::getComponent(const std::string &name) -> std::optional<TComponent &>
{
    auto &component = m_components[name];

    if (component == nullptr)
    {
        return std::optional<TComponent>();
    }

    return component;
}
} // namespace SSGE