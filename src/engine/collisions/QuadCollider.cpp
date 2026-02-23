#include "QuadCollider.h"
#include "CircleCollider.h"
#include "scenes/GameObject.h"

namespace SSGE
{
QuadCollider::QuadCollider(bool isPrimary, GameObject *gameObject, const glm::vec2 &topLeft, float width, float height)
    : Collider(isPrimary, gameObject, ColliderType::Quad, typeid(QuadCollider).name(), "Quad Collider"),
      m_topLeft(topLeft), m_width(width), m_height(height)
{
    bindFields();
}

auto QuadCollider::bindFields() -> void
{
    m_fields.push_back(std::make_unique<TypedComponentField<glm::vec2>>(
        "TopLeft", ComponentField::FieldType::Vec2, [this]() { return this->getTopLeft(); },
        [this](auto &topLeft) { this->setTopLeft(topLeft); }));
    m_fields.push_back(std::make_unique<TypedComponentField<float>>(
        "Width", ComponentField::FieldType::Float, [this]() { return this->getWidth(); },
        [this](auto &width) { this->setDimensions(width, m_height); }));
    m_fields.push_back(std::make_unique<TypedComponentField<float>>(
        "Height", ComponentField::FieldType::Float, [this]() { return this->getHeight(); },
        [this](auto &height) { this->setDimensions(m_width, height); }));
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

extern "C"
{
    SSGE_API auto QuadCollider_Create(bool isPrimary, SSGE::GameObject *gameObject, float topLeftX, float topLeftY,
                                      float width, float height) -> SSGE::QuadCollider *
    {
        auto &component = gameObject->addComponent<SSGE::QuadCollider>(isPrimary, gameObject,
                                                                       glm::vec2(topLeftX, topLeftY), width, height);
        return &component;
    }

    SSGE_API auto QuadCollider_GetTopLeft(SSGE::QuadCollider *collider, float *topLeftArray) -> void
    {
        auto topLeft = collider->getTopLeft();
        topLeftArray[0] = topLeft.x;
        topLeftArray[1] = topLeft.y;
    }

    SSGE_API auto QuadCollider_SetTopLeft(SSGE::QuadCollider *collider, float topLeftX, float topLeftY) -> void
    {
        collider->setTopLeft(glm::vec2(topLeftX, topLeftY));
    }

    SSGE_API auto QuadCollider_GetWidth(SSGE::QuadCollider *collider) -> float
    {
        return collider->getWidth();
    }

    SSGE_API auto QuadCollider_GetHeight(SSGE::QuadCollider *collider) -> float
    {
        return collider->getHeight();
    }

    SSGE_API auto QuadCollider_SetDimensions(SSGE::QuadCollider *collider, float width, float height) -> void
    {
        collider->setDimensions(width, height);
    }
}
