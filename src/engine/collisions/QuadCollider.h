#pragma once
#include "Collider.h"

namespace SSGE
{
class CircleCollider;
class QuadCollider final : public Collider
{
  public:
    QuadCollider(bool isPrimary, GameObject *gameObject, const glm::vec2 &topLeft, float width, float height);
    ~QuadCollider() override;

    auto checkCollision(Collider *other) -> std::optional<CollisionInfo> override;

    auto setTopLeft(const glm::vec2 &topLeft) -> void;
    auto setDimensions(float width, float height) -> void;

    [[nodiscard]] auto getTopLeft() const -> glm::vec2;
    [[nodiscard]] auto getWidth() const -> float;
    [[nodiscard]] auto getHeight() const -> float;

  private:
    static std::optional<glm::vec2> checkCollisionWithQuad(QuadCollider *other);
    static std::optional<glm::vec2> checkCollisionWithCircle(CircleCollider *other);

  public:
    auto init() -> void override;
    auto update() -> void override;

  private:
    glm::vec2 m_topLeft;
    float m_width;
    float m_height;
};
} // namespace SSGE