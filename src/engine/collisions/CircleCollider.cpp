#include "CircleCollider.h"
#include "QuadCollider.h"
#include "scenes/GameObject.h"
#include <set>

namespace SSGE
{
CircleCollider::CircleCollider(bool isPrimary, GameObject *gameObject, const glm::vec2 &center, float radius)
    : Collider(isPrimary, gameObject, Circle), m_center(center), m_radius(radius)
{
}

CircleCollider::~CircleCollider() = default;

std::optional<CollisionInfo> CircleCollider::checkCollision(Collider *other)
{
    switch (other->getType())
    {
    case Collider::Quad: {
        auto quad = dynamic_cast<QuadCollider *>(other);
        auto contactPoint = checkCollisionWithQuad(quad);
        if (contactPoint.has_value() && !m_collisions.contains(quad))
        {
            m_collisions.insert(quad);
            CollisionInfo info = {};
            info.collider = const_cast<CircleCollider *>(this);
            info.other = quad;
            info.contactPoint = contactPoint.value();
            info.entered = true;
            return info;
        }
        if (!contactPoint.has_value() && m_collisions.contains(quad))
        {
            m_collisions.erase(quad);
            CollisionInfo info = {};
            info.collider = this;
            info.other = quad;
            info.entered = false;
            return info;
        }
        break;
    }
    case Circle: {
        throw std::runtime_error("Circle vs Circle collision not implemented");
    }
    default:
        break;
    }

    return {};
}

void CircleCollider::setCenter(const glm::vec2 &center)
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

auto CircleCollider::init() -> void
{
}

auto CircleCollider::update() -> void
{
}

std::optional<glm::vec2> CircleCollider::checkCollisionWithQuad(QuadCollider *quad) const
{
    glm::vec3 circlePosition = gameObject()->getConstTransform().position();
    glm::vec3 quadPosition = quad->gameObject()->getConstTransform().position();
    glm::vec2 actualCenter = m_center + glm::vec2(circlePosition.x, circlePosition.y);
    glm::vec2 actualQuadTopLeft = quad->getTopLeft() + glm::vec2(quadPosition.x, quadPosition.y);

    if (actualCenter.x - m_radius < actualQuadTopLeft.x + quad->getWidth() &&
        actualCenter.x + m_radius > actualQuadTopLeft.x && actualCenter.y - m_radius > actualQuadTopLeft.y &&
        actualCenter.y + m_radius < actualQuadTopLeft.y + quad->getHeight())
    {
        return glm::vec2(quad->getTopLeft().x, m_center.y);
    }

    return {};
}
} // namespace SSGE

extern "C"
{
    SSGE_API auto CircleCollider_Create(bool isPrimary, SSGE::GameObject *gameObject, float centerX, float centerY,
                                        float radius) -> SSGE::CircleCollider *
    {
        auto &component =
            gameObject->addComponent<SSGE::CircleCollider>(isPrimary, gameObject, glm::vec2(centerX, centerY), radius);
        return &component;
    }

    SSGE_API auto CircleCollider_GetCenter(SSGE::CircleCollider *collider, float *centerArray) -> void
    {
        auto center = collider->getCenter();
        centerArray[0] = center.x;
        centerArray[1] = center.y;
    }

    SSGE_API auto CircleCollider_SetCenter(SSGE::CircleCollider *collider, float centerX, float centerY) -> void
    {
        collider->setCenter(glm::vec2(centerX, centerY));
    }

    SSGE_API auto CircleCollider_GetRadius(SSGE::CircleCollider *collider) -> float
    {
        return collider->getRadius();
    }

    SSGE_API auto CircleCollider_SetRadius(SSGE::CircleCollider *collider, float radius) -> void
    {
        collider->setRadius(radius);
    }
}
