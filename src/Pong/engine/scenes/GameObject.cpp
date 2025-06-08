#include "GameObject.h"

#include "Game.h"

#include <ranges>
#include <utility>

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

template <Derived<Component> TComponent> auto GameObject::getComponent() -> std::optional<TComponent *>
{
    const char *name = typeid(TComponent).name();

    const auto &component = m_components[name];

    if (component == nullptr)
    {
        return std::optional<TComponent>();
    }

    return *component.get();
}

template <Derived<Component> TComponent> auto GameObject::getComponents() -> std::vector<TComponent *>
{
    auto range =  std::views::filter(m_components | std::views::values | std::views::transform([](auto &component) {
        return dynamic_cast<TComponent*>(component.get());
    }, [](TComponent* component) {
        return component != nullptr;
    }) ) | std::views::transform([](TComponent* component) {return *component;});

    return std::vector<TComponent &>(range.begin(), range.end());
}

template <Derived<Component> TComponent, class... TArgs> auto GameObject::addComponent(TArgs &&...args) -> TComponent &
{
    std::unique_ptr<TComponent> component = std::make_unique<TComponent>(std::forward<TArgs>(args)...);

    auto rawComponent = component.get();

    m_components.insert(std::make_pair(component->getName(), component));

    return *rawComponent;
}
} // namespace SSGE