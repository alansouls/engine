#include "Collider.h"
#include "scenes/Game.h"

namespace SSGE
{
Collider::Collider(GameObject *gameObject, ColliderType type, std::string name, std::string displayName,
                   Component::ComponentType componentType)
    : Component(std::move(name), std::move(displayName), gameObject, componentType), m_type(type), m_isPrimary(false),
      m_messenger(nullptr)
{
    m_messenger = Game::getInstance()->messenger();
}

Collider::~Collider()
{
    m_messenger->send(ColliderRemovedMessage{.collider = this});
}

[[nodiscard]] auto Collider::getType() const -> ColliderType
{
    return m_type;
}

[[nodiscard]] auto Collider::isPrimary() const -> bool
{
    return m_isPrimary;
}

auto Collider::setIsPrimary(bool isPrimary) -> void
{
    if (m_isPrimary == isPrimary)
        return;
    IsPrimaryChangedMessage message{.collider = this, .oldIsPrimary = m_isPrimary};
    m_isPrimary = isPrimary;
    m_messenger->send(message);
}

auto Collider::setLayer(const std::string &layer) -> void
{
    if (m_layer == layer)
        return;
    LayerChangedMessage message{.collider = this, .oldLayer = m_layer};
    m_layer = layer;
    m_messenger->send(message);
}

[[nodiscard]] auto Collider::getLayer() const -> const std::string &
{
    return m_layer;
}

auto Collider::setCollidesWith(const std::vector<std::string> &collidesWith) -> void
{
    m_collidesWith = collidesWith;
}

[[nodiscard]] auto Collider::getCollidesWith() const -> const std::vector<std::string> &
{
    return m_collidesWith;
}

auto Collider::onCollisionEnter(const CollisionInfo &other) const -> void
{
    m_onCollisionEnterCallback(other);
}

auto Collider::onCollisionExit(const CollisionInfo &other) const -> void
{
    m_onCollisionExitCallback(other);
}

auto Collider::setOnCollisionEnterCallback(const std::function<void(const CollisionInfo &)> &callback) -> void
{
    m_onCollisionEnterCallback = callback;
}

auto Collider::setOnCollisionExitCallback(const std::function<void(const CollisionInfo &)> &callback) -> void
{
    m_onCollisionExitCallback = callback;
}

auto Collider::bindFields() -> void
{
    m_fields.push_back(std::make_unique<TypedComponentField<bool>>(
        "IsPrimary", ComponentField::FieldType::Bool, [this]() { return isPrimary(); },
        [this](const bool &value) { setIsPrimary(value); }));
    m_fields.push_back(std::make_unique<TypedComponentField<std::string>>(
        "Layer", ComponentField::FieldType::String, [this]() { return getLayer(); },
        [this](const std::string &value) { setLayer(value); }));
}
} // namespace SSGE

extern "C"
{
    SSGE_API auto Collider_GetLayer(SSGE::Collider *collider) -> const char *
    {
        return collider->getLayer().c_str();
    }

    SSGE_API auto Collider_SetLayer(SSGE::Collider *collider, const char *layer) -> void
    {
        collider->setLayer(layer);
    }

    SSGE_API auto Collider_SetCollidesWith(SSGE::Collider *collider, const char **layers, int layersLength) -> void
    {
        std::vector<std::string> layersVector;
        layersVector.reserve(layersLength);
        for (int i = 0; i < layersLength; i++)
        {
            layersVector.emplace_back(layers[i]);
        }
        collider->setCollidesWith(layersVector);
    }

    SSGE_API auto Collider_RegisterOnCollisionEnterCallback(SSGE::Collider *collider,
                                                            void (*callback)(SSGE::CollisionInfo *)) -> void
    {
        collider->setOnCollisionEnterCallback([callback](const SSGE::CollisionInfo &collisionInfo) {
            callback(const_cast<SSGE::CollisionInfo *>(&collisionInfo));
        });
    }

    SSGE_API auto Collider_RegisterOnCollisionExitCallback(SSGE::Collider *collider,
                                                           void (*callback)(SSGE::CollisionInfo *)) -> void
    {
        collider->setOnCollisionExitCallback([callback](const SSGE::CollisionInfo &collisionInfo) {
            callback(const_cast<SSGE::CollisionInfo *>(&collisionInfo));
        });
    }
}
