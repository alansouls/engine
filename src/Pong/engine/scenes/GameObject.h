#pragma once

#include "Component.h"
#include "GameProperties.h"
#include "engine/compile_utils/ssge_concepts.h"
#include "engine/graphics/utils/Transform.h"

#include <unordered_map>

class Collider;
class CollisionInfo;
class RendererItem;
namespace SSGE
{
class GameObject final : Component
{
  public:
    explicit GameObject(std::string name, const std::optional<GameObject *> &parent = {});
    ~GameObject() override = default;
    auto init() -> void override;
    auto update() -> void override;
    auto getName() -> const std::string &;
    auto gameObject() -> GameObject & override;

    template <Derived<Component> TComponent> auto getComponent(const std::string &name) -> std::optional<TComponent &>;

    template <Derived<Component> TComponent, class... TArgs> auto addComponent(TArgs &&...args) -> TComponent &;

    [[nodiscard]] auto getConstTransform() const -> const Transform &;
    [[nodiscard]] auto getTransform() -> Transform &;

  protected:
    static auto getGameProperties() -> GameProperties;

  private:
    std::unordered_map<std::string, std::unique_ptr<Component>> m_components;
    std::vector<std::unique_ptr<GameObject>> m_children;
    std::optional<GameObject *> m_parent;
    std::string m_name;
    Transform m_transform;
};
} // namespace SSGE
