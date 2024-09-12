#pragma once
#include "Collider.h"

class CircleCollider;
class QuadCollider : public Collider
{
public:
	QuadCollider(bool isPrimary, GameObject* gameObject, const glm::vec2 &topLeft, float width, float height);
	~QuadCollider() override;

	std::optional<CollisionInfo> checkCollision(Collider* other) override;

	void setTopLeft(const glm::vec2 &topLeft);
	void setDimensions(float width, float height);

	glm::vec2 getTopLeft() const;
	float getWidth() const;
	float getHeight() const;
private:
	std::optional<glm::vec2> checkCollisionWithQuad(QuadCollider* other) const;
	std::optional<glm::vec2> checkCollisionWithCircle(CircleCollider* other) const;

	glm::vec2 m_topLeft;
	float m_width;
	float m_height;
};