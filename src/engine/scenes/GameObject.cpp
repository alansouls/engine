#include "GameObject.h"

#include "Game.h"
#include <memory>

namespace SSGE
{
GameObject::GameObject(std::string name, const std::optional<GameObject *> &parent)
    : m_parent(parent), m_name(std::move(name))
{
}

auto GameObject::init() -> void
{
    m_transform = m_initialTransform;

    for (auto &component : m_components | std::views::values)
    {
        m_componentsToInit.insert(component.get());
        component->applyInitialValues();
    }
}

auto GameObject::update() -> void
{
    while (!m_componentsToInit.empty())
    {
        std::vector<Component *> initializedComponents;
        for (Component *component : m_componentsToInit)
        {
            component->init();
            initializedComponents.push_back(component);
        }

        for (auto component : initializedComponents)
        {
            m_componentsToInit.erase(component);
        }
    }

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

auto GameObject::getInitialTransform() -> Transform &
{
    return m_initialTransform;
}

auto GameObject::components() const -> std::vector<Component *>
{
    std::vector<Component *> components;
    for (auto &component : m_components | std::views::values)
    {
        components.push_back(component.get());
    }

    return components;
}

GameProperties GameObject::getGameProperties()
{
    return Game::getInstance()->getProperties();
}

auto GameObject::name() const -> const std::string &
{
    return m_name;
}

auto GameObject::setName(std::string name) -> void
{
    m_name = std::move(name);
}

auto GameObject::removeComponent(const std::string &name) -> void
{
    std::unique_ptr<Component> &component = m_components[name];
    if (component)
    {
        m_componentsToInit.erase(component.get());
    }
    m_components.erase(name);
}

} // namespace SSGE
