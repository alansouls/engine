#pragma once

#include "Collider.h"

namespace SSGE
{
class QuadCollider;
class CircleCollider final : public Collider
{
  public:
    CircleCollider(bool isPrimary, GameObject *gameObject, const glm::vec2 &center, float radius);
    ~CircleCollider() override;

    auto checkCollision(Collider *other) -> std::optional<CollisionInfo> override;

    void setCenter(const glm::vec2 &center);
    void setRadius(float radius);

    [[nodiscard]] auto getCenter() const -> glm::vec2;
    [[nodiscard]] auto getRadius() const -> float;

    auto init() -> void override;
    auto update() -> void override;

  private:
    glm::vec2 m_center;
    float m_radius;

    std::optional<glm::vec2> checkCollisionWithQuad(QuadCollider *quad) const;
};
} // namespace SSGE