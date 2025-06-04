#include "GameObject.h"

#include "Game.h"

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

auto GameObject::getTransform() const -> const Transform &
{
    return m_transform;
}

auto GameObject::getMutTransform() -> Transform &
{
    return m_transform;
}

GameProperties GameObject::getGameProperties()
{
    return Game::getInstance()->getProperties();
}

template <Derived<Component> TComponent>
auto GameObject::getComponent(const std::string &name) -> std::optional<TComponent &>
{
    auto &component = m_components[name];

    if (component == nullptr)
    {
        return std::optional<TComponent>();
    }

    return component;
}

template <Derived<Component> TComponent, class... TArgs> auto GameObject::addComponent(TArgs &&...args) -> TComponent &
{
    std::unique_ptr<TComponent> component = std::make_unique<TComponent>(std::forward<TArgs>(args)...);

    auto rawComponent = component.get();

    m_components.insert(std::make_pair(component->getName(), component));

    return *rawComponent;
}
} // namespace SSGE