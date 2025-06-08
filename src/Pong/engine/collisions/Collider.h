#pragma once
#include "CollisionInfo.h"
#include "engine/scenes/Component.h"

#include <functional>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace SSGE
{
class GameObject;
class Collider : Component
{
  public:
    enum ColliderType
    {
        Quad,
        Circle
    };

    static const std::string TypeName;

    Collider(bool isPrimary, GameObject *gameObject, ColliderType type)
        : Component(TypeName), m_type(type), m_isPrimary(isPrimary), m_gameObject(gameObject)
    {
    }

    virtual ~Collider() = 0;

    virtual std::optional<CollisionInfo> checkCollision(Collider *other) = 0;

    ColliderType getType() const
    {
        return m_type;
    }

    GameObject *getGameObject() const
    {
        return m_gameObject;
    }

    bool isPrimary() const
    {
        return m_isPrimary;
    }

    void setLayer(const std::string &layer)
    {
        m_layer = layer;
    }
    const std::string &getLayer() const
    {
        return m_layer;
    }

    void setCollidesWith(const std::vector<std::string> &collidesWith)
    {
        m_collidesWith = collidesWith;
    }

    const std::vector<std::string> &getCollidesWith() const
    {
        return m_collidesWith;
    }

    auto onCollisionEnter(const CollisionInfo &other) const -> void
    {
        for (const auto &callback : m_onCollisionEnterCallbacks)
        {
            callback(other);
        }
    }

    auto onCollisionExit(const CollisionInfo &other) const -> void
    {
        for (const auto &callback : m_onCollisionExitCallbacks)
        {
            callback(other);
        }
    }

    auto addOnCollisionEnterCallback(const std::function<void(const CollisionInfo &)> &callback) -> void
    {
        m_onCollisionEnterCallbacks.push_back(callback);
    }

    auto addOnCollisionExitCallback(const std::function<void(const CollisionInfo &)> &callback) -> void
    {
        m_onCollisionExitCallbacks.push_back(callback);
    }

  protected:
    std::set<Collider *> m_collisions;

  private:
    ColliderType m_type;
    bool m_isPrimary;
    GameObject *m_gameObject;
    std::string m_layer;
    std::vector<std::string> m_collidesWith;
    std::vector<std::function<void(const CollisionInfo &)>> m_onCollisionEnterCallbacks;
    std::vector<std::function<void(const CollisionInfo &)>> m_onCollisionExitCallbacks;
};

inline Collider::~Collider() = default;
const std::string Collider::TypeName = "ColliderComponent";
} // namespace SSGE