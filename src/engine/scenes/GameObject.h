#pragma once

#include "Component.h"
#include "GameProperties.h"
#include "engine/compile_utils/ssge_concepts.h"
#include "engine/graphics/utils/Transform.h"
#include <ranges>
#include <set>
#include <string_view>
#include <utility>

#include <unordered_map>

class RendererItem;
namespace SSGE
{
class Collider;
struct CollisionInfo;
class GameObject final
{
  public:
    struct ComponentRemovedMessage
    {
        static constexpr std::string_view Name = "GameObject_ComponentRemovedMessage";
        GameObject *gameObject;
        Component *componentName;
    };

    explicit GameObject(std::string name, const std::optional<GameObject *> &parent = {});
    auto init() -> void;
    auto update() -> void;
    auto getName() -> const std::string &;

    template <Derived<Component> TComponent> auto getComponent(const std::string &name) -> std::optional<TComponent *>;
    template <Derived<Component> TComponent> auto getComponent() -> std::optional<TComponent *>;
    template <Derived<Component> TComponent> auto getComponents() -> std::vector<TComponent *>;

    template <Derived<Component> TComponent, class... TArgs> auto addComponent(TArgs &&...args) -> TComponent &;

    [[nodiscard]] auto getConstTransform() const -> const Transform &;
    [[nodiscard]] auto getTransform() -> Transform &;
    [[nodiscard]] auto getInitialTransform() -> Transform &;

    auto components() const -> std::vector<Component *>;

    auto name() const -> const std::string &;
    auto setName(std::string name) -> void;

    static auto getGameProperties() -> GameProperties;

    auto removeComponent(const std::string &name) -> void;

  private:
    std::unordered_map<std::string, std::unique_ptr<Component>> m_components;
    std::set<Component *> m_componentsToInit;
    std::vector<std::unique_ptr<GameObject>> m_children;
    std::optional<GameObject *> m_parent;
    std::string m_name;
    Transform m_transform;
    Transform m_initialTransform;
};

template <Derived<Component> TComponent>
auto GameObject::getComponent(const std::string &name) -> std::optional<TComponent *>
{
    const auto &it = m_components.find(name);

    if (it == m_components.end())
        return std::nullopt;

    auto finalComponent = dynamic_cast<TComponent *>(it->second.get());

    if (!finalComponent)
    {
        return std::nullopt;
    }

    return std::optional<TComponent *>(dynamic_cast<TComponent *>(it->second.get()));
}

template <Derived<Component> TComponent> auto GameObject::getComponent() -> std::optional<TComponent *>
{
    const char *name = typeid(TComponent).name();

    const auto &component = m_components[name];

    if (component == nullptr)
    {
        return std::optional<TComponent *>();
    }

    return std::optional<TComponent *>(dynamic_cast<TComponent *>(component.get()));
}

template <Derived<Component> TComponent> auto GameObject::getComponents() -> std::vector<TComponent *>
{
    auto range = std::views::filter(m_components | std::views::values |
                                        std::views::transform([](const std::unique_ptr<Component> &component) {
                                            return dynamic_cast<TComponent *>(component.get());
                                        }),
                                    [](TComponent *component) { return component != nullptr; });

    return std::vector<TComponent *>(range.begin(), range.end());
}

template <Derived<Component> TComponent, class... TArgs> auto GameObject::addComponent(TArgs &&...args) -> TComponent &
{
    std::unique_ptr<TComponent> component = std::make_unique<TComponent>(std::forward<TArgs>(args)...);

    TComponent *rawComponent = component.get();

    m_components[rawComponent->name()] = std::move(component);
    m_componentsToInit.insert(rawComponent);

    return *rawComponent;
}

} // namespace SSGE
