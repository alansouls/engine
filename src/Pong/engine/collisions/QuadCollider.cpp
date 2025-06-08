#include "QuadCollider.h"
#include "CircleCollider.h"

namespace SSGE
{
QuadCollider::QuadCollider(bool isPrimary, GameObject *gameObject, const glm::vec2 &topLeft, float width, float height)
    : Collider(isPrimary, gameObject, ColliderType::Quad), m_topLeft(topLeft), m_width(width), m_height(height)
{
}

QuadCollider::~QuadCollider() = default;

std::optional<CollisionInfo> QuadCollider::checkCollision(Collider *other)
{
    // switch (other->getType())
    // {
    // default:
    //     break;
    // }

    return {};
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

std::optional<glm::vec2> QuadCollider::checkCollisionWithQuad(QuadCollider *other)
{
    return glm::vec2();
}

std::optional<glm::vec2> QuadCollider::checkCollisionWithCircle(CircleCollider *other)
{
    return {};
}

auto QuadCollider::init() -> void
{
}

auto QuadCollider::update() -> void
{
}
} // namespace SSGE
