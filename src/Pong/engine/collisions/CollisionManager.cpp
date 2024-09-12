#include "CollisionManager.h"
#include "../scenes/GameObject.h"
#include "Collider.h"

void CollisionManager::checkCollisions(const std::vector<GameObject*>& gameObjects)
{
	for (auto primaryGameObject : gameObjects) {
		auto collider = primaryGameObject->getCollider();
		if (collider == nullptr || !collider->isPrimary())
			continue;

		for (auto secondaryGameObject : gameObjects) {
			if (primaryGameObject == secondaryGameObject)
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
