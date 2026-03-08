#pragma once
#include "Collider.h"
#include "EngineAPI.h"

namespace SSGE
{
class CircleCollider;
class QuadCollider final : public Collider
{
  public:
    QuadCollider(GameObject *gameObject);
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
    glm::vec2 m_topLeft{};
    float m_width;
    float m_height;

    auto bindFields() -> void override;
};
} // namespace SSGE

extern "C"
{
    SSGE_API auto QuadCollider_Create(bool isPrimary, SSGE::GameObject *gameObject, float topLeftX, float topLeftY,
                                      float width, float height) -> SSGE::QuadCollider *;

    SSGE_API auto QuadCollider_GetTopLeft(SSGE::QuadCollider *collider, float *topLeftArray) -> void;

    SSGE_API auto QuadCollider_SetTopLeft(SSGE::QuadCollider *collider, float topLeftX, float topLeftY) -> void;

    SSGE_API auto QuadCollider_GetWidth(SSGE::QuadCollider *collider) -> float;

    SSGE_API auto QuadCollider_GetHeight(SSGE::QuadCollider *collider) -> float;

    SSGE_API auto QuadCollider_SetDimensions(SSGE::QuadCollider *collider, float width, float height) -> void;
}
