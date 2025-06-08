#include "CollisionManager.h"
#include "../scenes/GameObject.h"
#include "Collider.h"

void CollisionManager::checkCollisions()
{
    for (const auto &primaryGameObject : m_primaryColliders)
    {
        auto collider = primaryGameObject->getComponent<SSGE::Collider>(SSGE::Collider::TypeName);
        if (collider.has_value() || !collider.value().isPrimary())
            continue;

        for (auto &layer : collider.value().getCollidesWith())
        {
            const auto layerIter = m_collidersByLayer.find(layer);
            if (!m_collidersByLayer.contains(layer))
                continue;
            for (auto &secondaryGameObject : layerIter->second)
            {
                if (secondaryGameObject == primaryGameObject)
                    continue;

                auto otherCollider = secondaryGameObject->getComponent<SSGE::Collider>(SSGE::Collider::TypeName);

                if (otherCollider.has_value())
                    continue;

                auto collisionInfo = collider.value().checkCollision(&otherCollider.value());

                if (!collisionInfo.has_value())
                    continue;

                if (collisionInfo.value().entered)
                {
                    collider.value().onCollisionEnter(collisionInfo.value());
                }
                else
                {
                    collider.value().onCollisionExit(collisionInfo.value());
                }
            }
        }
    }
}

void CollisionManager::addGameObjectCollider(const std::shared_ptr<SSGE::GameObject> &gameObject)
{
    if (!gameObject)
    {
        return;
    }

    auto collider = gameObject->getComponent<SSGE::Collider>(SSGE::Collider::TypeName);

    if (!collider.has_value())
    {
        return;
    }

    if (collider.value().isPrimary())
    {
        m_primaryColliders.push_back(gameObject);
    }

    m_collidersByLayer[collider.value().getLayer()].push_back(gameObject);
}

void CollisionManager::removeGameObjectCollider(const std::shared_ptr<SSGE::GameObject> &gameObject)
{
    if (!gameObject)
    {
        return;
    }

    auto collider = gameObject->getComponent<SSGE::Collider>(SSGE::Collider::TypeName);

    if (!collider.has_value())
    {
        return;
    }

    if (collider.value().isPrimary())
    {
        std::erase(m_primaryColliders, gameObject);
    }

    auto &colliders = m_collidersByLayer[collider.value().getLayer()];
    std::erase(colliders, gameObject);
}
