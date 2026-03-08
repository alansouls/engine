#pragma once

#include "CollisionInfo.h"
#include "EngineAPI.h"
#include "core/Messenger.h"
#include "engine/scenes/Component.h"
#include "scenes/ComponentField.h"
#include "scenes/Game.h"

#include <functional>
#include <memory>
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
             Component::ComponentType componentType)
        : Component(std::move(name), std::move(displayName), gameObject, componentType), m_type(type),
          m_isPrimary(false), m_messenger(nullptr)
    {
        m_messenger = Game::getInstance()->messenger();
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

    auto setIsPrimary(bool isPrimary) -> void
    {
        if (m_isPrimary == isPrimary)
            return;
        IsPrimaryChangedMessage message{.collider = this, .oldIsPrimary = m_isPrimary};
        m_isPrimary = isPrimary;
        m_messenger->send(message);
    }

    auto setLayer(const std::string &layer) -> void
    {
        if (m_layer == layer)
            return;
        LayerChangedMessage message{.collider = this, .oldLayer = m_layer};
        m_layer = layer;
        m_messenger->send(message);
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

    virtual auto bindFields() -> void
    {
        m_fields.push_back(std::make_unique<TypedComponentField<bool>>(
            "IsPrimary", ComponentField::FieldType::Bool, [this]() { return isPrimary(); },
            [this](const bool &value) { setIsPrimary(value); }));
        m_fields.push_back(std::make_unique<TypedComponentField<std::string>>(
            "Layer", ComponentField::FieldType::String, [this]() { return getLayer(); },
            [this](const std::string &value) { setLayer(value); }));
    }

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
