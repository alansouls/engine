#pragma once

#include "CollisionInfo.h"
#include "EngineAPI.h"
#include "core/Messenger.h"
#include "engine/scenes/Component.h"

#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace SSGE
{
class GameObject;
class Collider : public Component
{
  public:
    struct ColliderRemovedMessage
    {
        static constexpr std::string_view Name = "Collider_ColliderRemovedMessage";
        Collider *collider;
    };

    struct IsPrimaryChangedMessage
    {
        static constexpr std::string_view Name = "Collider_IsPrimaryChangedMessage";
        Collider *collider;
        bool oldIsPrimary;
    };

    struct LayerChangedMessage
    {
        static constexpr std::string_view Name = "Collider_LayerChangedMessage";
        Collider *collider;
        std::string oldLayer;
    };

    enum class ColliderType
    {
        Quad,
        Circle
    };

    Collider(GameObject *gameObject, ColliderType type, std::string name, std::string displayName,
             Component::ComponentType componentType);

    ~Collider() override;

    virtual auto checkCollision(Collider *other) -> std::optional<CollisionInfo> = 0;

    [[nodiscard]] auto getType() const -> ColliderType;

    [[nodiscard]] auto isPrimary() const -> bool;

    auto setIsPrimary(bool isPrimary) -> void;

    auto setLayer(const std::string &layer) -> void;

    [[nodiscard]] auto getLayer() const -> const std::string &;

    auto setCollidesWith(const std::vector<std::string> &collidesWith) -> void;

    [[nodiscard]] auto getCollidesWith() const -> const std::vector<std::string> &;

    auto onCollisionEnter(const CollisionInfo &other) const -> void;

    auto onCollisionExit(const CollisionInfo &other) const -> void;

    auto setOnCollisionEnterCallback(const std::function<void(const CollisionInfo &)> &callback) -> void;

    auto setOnCollisionExitCallback(const std::function<void(const CollisionInfo &)> &callback) -> void;

  protected:
    std::set<Collider *> m_collisions;

    virtual auto bindFields() -> void;

  private:
    ColliderType m_type;
    bool m_isPrimary;
    std::string m_layer;
    std::vector<std::string> m_collidesWith;
    std::function<void(const CollisionInfo &)> m_onCollisionEnterCallback{[](const CollisionInfo &) {}};
    std::function<void(const CollisionInfo &)> m_onCollisionExitCallback{[](const CollisionInfo &) {}};
    Messenger *m_messenger;
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
