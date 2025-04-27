#pragma once
#include "CollisionInfo.h"
#include <optional>
#include <set>
#include <string>
#include <vector>

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

	void setLayer(const std::string& layer) { m_layer = layer; }
	const std::string &getLayer() const { return m_layer; }

	void setCollidesWith(const std::vector<std::string>& collidesWith) { m_collidesWith = collidesWith; }
	const std::vector<std::string>& getCollidesWith() const { return m_collidesWith; }

protected:
	std::set<Collider*> m_collisions;

private:
	ColliderType m_type;
	bool m_isPrimary;
	GameObject* m_gameObject;
	std::string m_layer;
	std::vector<std::string> m_collidesWith;
};

inline Collider::~Collider() {}