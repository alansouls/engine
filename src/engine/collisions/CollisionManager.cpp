#include "CollisionManager.h"
#include "../scenes/GameObject.h"
#include "Collider.h"
#include "scenes/Game.h"

#include <algorithm>

namespace SSGE
{

CollisionManager::CollisionManager()
{
    auto messenger = Game::getInstance()->messenger();

    messenger->connect<Collider::IsPrimaryChangedMessage>(
        [this](const Collider::IsPrimaryChangedMessage &message) { onColliderIsPrimaryChanged(message); });

    messenger->connect<Collider::LayerChangedMessage>(
        [this](const Collider::LayerChangedMessage &message) { onColliderLayerChanged(message); });
}

void CollisionManager::checkCollisions()
{
    for (const auto &primaryGameObject : m_primaryColliders)
    {
        auto colliders = primaryGameObject->getComponents<SSGE::Collider>();

        if (colliders.empty())
            continue;

        SSGE::Collider &collider = *colliders.at(0);

        for (auto &layer : collider.getCollidesWith())
        {
            const auto layerIter = m_collidersByLayer.find(layer);
            if (!m_collidersByLayer.contains(layer))
                continue;
            for (auto &secondaryGameObject : layerIter->second)
            {
                if (secondaryGameObject == primaryGameObject)
                    continue;

                auto otherColliders = secondaryGameObject->getComponents<SSGE::Collider>();

                if (otherColliders.empty())
                    continue;

                SSGE::Collider &otherCollider = *otherColliders.at(0);

                auto collisionInfo = collider.checkCollision(&otherCollider);

                if (!collisionInfo.has_value())
                    continue;

                if (collisionInfo.value().entered)
                {
                    collider.onCollisionEnter(collisionInfo.value());
                }
                else
                {
                    collider.onCollisionExit(collisionInfo.value());
                }
            }
        }
    }
}

void CollisionManager::addGameObjectCollider(SSGE::GameObject *gameObject)
{
    if (!gameObject)
    {
        return;
    }

    auto colliders = gameObject->getComponents<SSGE::Collider>();

    if (colliders.empty())
    {
        return;
    }

    const auto &collider = *colliders.at(0);

    if (collider.isPrimary())
    {
        m_primaryColliders.push_back(gameObject);
    }

    m_collidersByLayer[collider.getLayer()].push_back(gameObject);
}

void CollisionManager::removeGameObjectCollider(SSGE::GameObject *gameObject)
{
    if (!gameObject)
    {
        return;
    }

    auto collider = gameObject->getComponent<SSGE::Collider>();

    if (!collider.has_value())
    {
        return;
    }

    if (collider.value()->isPrimary())
    {
        std::erase(m_primaryColliders, gameObject);
    }

    auto &colliders = m_collidersByLayer[collider.value()->getLayer()];
    std::erase(colliders, gameObject);
}

auto CollisionManager::clear() -> void
{
    m_primaryColliders.clear();
    m_collidersByLayer.clear();
}

auto CollisionManager::onColliderIsPrimaryChanged(const Collider::IsPrimaryChangedMessage &message) -> void
{
    if (message.oldIsPrimary)
    {
        const auto toRemove = std::ranges::remove(m_primaryColliders, message.collider->gameObject());
        m_primaryColliders.erase(toRemove.begin(), toRemove.end());
    }
    else
    {
        m_primaryColliders.push_back(message.collider->gameObject());
    }
}

auto CollisionManager::onColliderLayerChanged(const Collider::LayerChangedMessage &message) -> void
{
    const auto oldLayerIt = m_collidersByLayer.find(message.oldLayer);
    if (oldLayerIt != m_collidersByLayer.end())
    {
        std::vector<GameObject *> &gameObjects = oldLayerIt->second;
        const auto toRemove = std::ranges::remove(gameObjects, message.collider->gameObject());
        gameObjects.erase(toRemove.begin(), toRemove.end());
    }

    m_collidersByLayer[message.collider->getLayer()].push_back(message.collider->gameObject());
}

} // namespace SSGE
