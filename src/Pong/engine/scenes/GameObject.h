#pragma once

#include "Component.h"
#include "GameProperties.h"

class Collider;
class CollisionInfo;
class RendererItem;
namespace SSGE
{
class GameObject final : Component
{
  public:
    explicit GameObject(std::string name, const std::optional<GameObject &> &parent = {});
    ~GameObject() override = default;
    auto init() -> void override;
    auto update() -> void override;
    auto getName() -> const std::string &;
    auto gameObject() -> GameObject & override;

    template <class TComponent> auto getComponent(const std::string &name) -> std::optional<TComponent &>;

  protected:
    static auto getGameProperties() -> GameProperties;

  private:
    std::unordered_map<std::string, std::unique_ptr<Component>> m_components;
    std::vector<std::unique_ptr<GameObject>> m_children;
    std::optional<GameObject &> m_parent;
    std::string m_name;
};
} // namespace SSGE
