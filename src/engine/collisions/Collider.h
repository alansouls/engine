#pragma once

#include "CollisionInfo.h"
#include "EngineAPI.h"
#include "engine/scenes/Component.h"

#include <functional>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace SSGE
{
class GameObject;
class Collider : public Component
{
  public:
    enum ColliderType
    {
        Quad,
        Circle
    };

    Collider(bool isPrimary, GameObject *gameObject, ColliderType type, std::string name, std::string displayName)
        : Component(std::move(name), std::move(displayName), gameObject), m_type(type), m_isPrimary(isPrimary)
    {
    }

    ~Collider() override = default;

    virtual auto checkCollision(Collider *other) -> std::optional<CollisionInfo> = 0;

    [[nodiscard]] auto getType() const -> ColliderType
    {
        return m_type;
    }

    [[nodiscard]] auto isPrimary() const -> bool
    {
        return m_isPrimary;
    }

    auto setLayer(const std::string &layer) -> void
    {
        m_layer = layer;
    }

    [[nodiscard]] auto getLayer() const -> const std::string &
    {
        return m_layer;
    }

    auto setCollidesWith(const std::vector<std::string> &collidesWith) -> void
    {
        m_collidesWith = collidesWith;
    }

    [[nodiscard]] auto getCollidesWith() const -> const std::vector<std::string> &
    {
        return m_collidesWith;
    }

    auto onCollisionEnter(const CollisionInfo &other) const -> void
    {
        m_onCollisionEnterCallback(other);
    }

    auto onCollisionExit(const CollisionInfo &other) const -> void
    {
        m_onCollisionExitCallback(other);
    }

    auto setOnCollisionEnterCallback(const std::function<void(const CollisionInfo &)> &callback) -> void
    {
        m_onCollisionEnterCallback = callback;
    }

    auto setOnCollisionExitCallback(const std::function<void(const CollisionInfo &)> &callback) -> void
    {
        m_onCollisionExitCallback = callback;
    }

  protected:
    std::set<Collider *> m_collisions;

  private:
    ColliderType m_type;
    bool m_isPrimary;
    std::string m_layer;
    std::vector<std::string> m_collidesWith;
    std::function<void(const CollisionInfo &)> m_onCollisionEnterCallback{[](const CollisionInfo &) {}};
    std::function<void(const CollisionInfo &)> m_onCollisionExitCallback{[](const CollisionInfo &) {}};
};
} // namespace SSGE

extern "C"
{
    SSGE_API auto Collider_GetLayer(SSGE::Collider *collider) -> const char *;

    SSGE_API auto Collider_SetLayer(SSGE::Collider *collider, const char *layer) -> void;

    SSGE_API auto Collider_SetCollidesWith(SSGE::Collider *collider, const char **layers, int layersLength) -> void;

    SSGE_API auto Collider_RegisterOnCollisionEnterCallback(SSGE::Collider *collider,
                                                            void (*callback)(SSGE::CollisionInfo *)) -> void;

    SSGE_API auto Collider_RegisterOnCollisionExitCallback(SSGE::Collider *collider,
                                                           void (*callback)(SSGE::CollisionInfo *)) -> void;
}
