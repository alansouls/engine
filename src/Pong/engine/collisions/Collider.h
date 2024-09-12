#pragma once
#include "CollisionInfo.h"
#include <optional>
#include <set>

class GameObject;
class Collider
{
public:
	enum ColliderType {
		Quad,
		Circle
	};

	Collider(bool isPrimary, GameObject *gameObject, ColliderType type) : m_type(type), m_isPrimary(isPrimary), m_gameObject(gameObject), m_collisions() {}
	virtual ~Collider() = 0;

	virtual std::optional<CollisionInfo> checkCollision(Collider* other) = 0;

	ColliderType getType() const { return m_type; }

	GameObject* getGameObject() const { return m_gameObject; }

	bool isPrimary() const { return m_isPrimary; }

protected:
	std::set<Collider*> m_collisions;

private:
	ColliderType m_type;
	bool m_isPrimary;
	GameObject* m_gameObject;
};

inline Collider::~Collider() {}