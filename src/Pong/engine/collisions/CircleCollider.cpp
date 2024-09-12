#include "CircleCollider.h"
#include "QuadCollider.h"
#include <set>

#include <iostream>

CircleCollider::CircleCollider(bool isPrimary, GameObject* gameObject, const glm::vec2& center, float radius) : 
	Collider(isPrimary, gameObject, ColliderType::Circle), m_center(center), m_radius(radius)
{
}

CircleCollider::~CircleCollider()
{
}

std::optional<CollisionInfo> CircleCollider::checkCollision(Collider* other) {
	switch (other->getType())
	{
	case Collider::Quad:
	{
		auto quad = dynamic_cast<QuadCollider*>(other);
		auto contactPoint = checkCollisionWithQuad(quad);
		if (contactPoint.has_value() && !m_collisions.contains(quad))
		{
			m_collisions.insert(quad);
			CollisionInfo info = {};
			info.collider = const_cast<CircleCollider*>(this);
			info.other = quad;
			info.contactPoint = contactPoint.value();
			info.entered = true;
			return info;
		}
		else if (!contactPoint.has_value() && m_collisions.contains(quad)) {
			m_collisions.erase(quad);
			CollisionInfo info = {};
			info.collider = const_cast<CircleCollider*>(this);
			info.other = quad;
			info.entered = false;
			return info;
		}
		break;
	}
	case Collider::Circle:
	{
		std::runtime_error("Circle vs Circle collision not implemented");
		break;
	}
	default:
		break;
	}

	return std::optional<CollisionInfo>();
}

void CircleCollider::setCenter(const glm::vec2& center)
{
	m_center = center;
}

void CircleCollider::setRadius(float radius)
{
	m_radius = radius;
}

glm::vec2 CircleCollider::getCenter() const
{
	return m_center;
}

float CircleCollider::getRadius() const
{
	return m_radius;
}

std::optional<glm::vec2> CircleCollider::checkCollisionWithQuad(QuadCollider* quad) const
{
	if ((m_center.y - m_radius < quad->getTopLeft().y) || (m_center.y + m_radius > quad->getTopLeft().y + quad->getHeight()))
	{
		return std::optional<glm::vec2>();
	}

	if ((m_center.x - m_radius < quad->getTopLeft().x) || (m_center.x + m_radius > quad->getTopLeft().x + quad->getWidth()))
	{
		return std::optional<glm::vec2>();
	}

	return glm::vec2(quad->getTopLeft().x, m_center.y);
}
