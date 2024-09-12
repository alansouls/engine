#pragma once

#include "Collider.h"

class QuadCollider;
class CircleCollider : public Collider
{
public:
	CircleCollider(bool isPrimary, GameObject* gameObject, const glm::vec2& center, float radius);
	~CircleCollider();

	std::optional<CollisionInfo> checkCollision(Collider* other) override;

	void setCenter(const glm::vec2& center);
	void setRadius(float radius);

	glm::vec2 getCenter() const;
	float getRadius() const;
private:
	glm::vec2 m_center;
	float m_radius;

	std::optional<glm::vec2> checkCollisionWithQuad(QuadCollider* quad) const;
};