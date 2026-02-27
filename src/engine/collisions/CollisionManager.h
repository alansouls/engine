#pragma once
#include "Collider.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace SSGE
{
class GameObject;
class CollisionManager
{
  public:
    CollisionManager();

    void checkCollisions();

    void addGameObjectCollider(SSGE::GameObject *gameObject);

    void removeGameObjectCollider(SSGE::GameObject *gameObject);

    auto clear() -> void;

  private:
    std::vector<SSGE::GameObject *> m_primaryColliders;
    std::unordered_map<std::string, std::vector<SSGE::GameObject *>> m_collidersByLayer;

    auto onColliderIsPrimaryChanged(const Collider::IsPrimaryChangedMessage &message) -> void;
    auto onColliderLayerChanged(const Collider::LayerChangedMessage &message) -> void;
};

} // namespace SSGE