#pragma once

#include "Collider.h"
#include "EngineAPI.h"

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

extern "C"
{
    SSGE_API auto CircleCollider_Create(bool isPrimary, SSGE::GameObject *gameObject, float centerX, float centerY,
                                        float radius) -> SSGE::CircleCollider *;

    SSGE_API auto CircleCollider_GetCenter(SSGE::CircleCollider *collider, float *centerArray) -> void;

    SSGE_API auto CircleCollider_SetCenter(SSGE::CircleCollider *collider, float centerX, float centerY) -> void;

    SSGE_API auto CircleCollider_GetRadius(SSGE::CircleCollider *collider) -> float;

    SSGE_API auto CircleCollider_SetRadius(SSGE::CircleCollider *collider, float radius) -> void;
}
