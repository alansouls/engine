#include "CollisionManager.h"
#include "../scenes/GameObject.h"
#include "Collider.h"

void CollisionManager::checkCollisions()
{
	for (auto primaryGameObject : m_primaryColliders) {
		auto collider = primaryGameObject->getCollider();
		if (collider == nullptr || !collider->isPrimary())
			continue;

		for (auto layer : collider->getCollidesWith()) {
			for (auto secondaryGameObject : m_collidersByLayer.at(layer)) {
				if (secondaryGameObject == primaryGameObject)
					continue;

				auto otherCollider = secondaryGameObject->getCollider();

				if (otherCollider == nullptr)
					continue;

				auto collisionInfo = collider->checkCollision(otherCollider);
				if (!collisionInfo.has_value())
					continue;

				if (collisionInfo.value().entered) {
					primaryGameObject->onCollisionEnter(collisionInfo.value());
				}
				else {
					primaryGameObject->onCollisionExit(collisionInfo.value());
				}
			}
		}
	}
}

void CollisionManager::addGameObjectCollider(GameObject* gameObject)
{
	if (!gameObject || !gameObject->getCollider()) {
		return;
	}

	auto collider = gameObject->getCollider();

	if (collider->isPrimary()) {
		m_primaryColliders.push_back(gameObject);
	}

	m_collidersByLayer[collider->getLayer()].push_back(gameObject);
}

void CollisionManager::removeGameObjectCollider(GameObject* gameObject)
{
	if (!gameObject || !gameObject->getCollider()) {
		return;
	}

	auto collider = gameObject->getCollider();

	if (collider->isPrimary()) {
		std::remove(m_primaryColliders.begin(), m_primaryColliders.end(), gameObject);
	}

	auto& colliders = m_collidersByLayer[collider->getLayer()];
	std::remove(colliders.begin(), colliders.end(), gameObject);
}
