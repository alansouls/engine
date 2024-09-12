#include "QuadCollider.h"
#include "CircleCollider.h"

QuadCollider::QuadCollider(bool isPrimary, GameObject* gameObject, const glm::vec2 &topLeft, float width, float height)
	: Collider(isPrimary, gameObject, ColliderType::Quad), m_topLeft(topLeft), m_width(width), m_height(height)
{
}

QuadCollider::~QuadCollider()
{
}

std::optional<CollisionInfo> QuadCollider::checkCollision(Collider* other)
{
	switch (other->getType())
	{
	case Collider::Quad:
	{
		break;
	}
	case Collider::Circle:
	{
		break;
	}
	default:
		break;
	}

	return std::optional<CollisionInfo>();
}

void QuadCollider::setTopLeft(const glm::vec2 &topLeft)
{
	m_topLeft = topLeft;
}

void QuadCollider::setDimensions(float width, float height)
{
	m_width = width;
	m_height = height;
}

glm::vec2 QuadCollider::getTopLeft() const
{
	return m_topLeft;
}

float QuadCollider::getWidth() const
{
	return m_width;
}

float QuadCollider::getHeight() const
{
	return m_height;
}

std::optional<glm::vec2> QuadCollider::checkCollisionWithQuad(QuadCollider* other) const
{
	return glm::vec2();
}

std::optional<glm::vec2> QuadCollider::checkCollisionWithCircle(CircleCollider* other) const
{
	return std::optional<glm::vec2>();
}
