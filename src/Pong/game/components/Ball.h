#pragma once
#include "../../engine/scenes/GameObject.h"
#include <glm/vec2.hpp>

namespace SSGE
{
class CircleCollider;
class CircleRendererComponent;
} // namespace SSGE
class Ball : public SSGE::Component
{
  public:
    explicit Ball(SSGE::GameObject *gameObject);
    void init() override;

    void update() override;

    void onKeyReleased(int key);

    void onCollisionEnter(const SSGE::CollisionInfo &info);
    void onCollisionExit(const SSGE::CollisionInfo &info);

  private:
    float m_originalWindowWidth;
    float m_originalWindowHeight;

    float m_lastWindowWidth;
    float m_lastWindowHeight;

    float m_radius;

    glm::vec2 m_direction;

    bool m_isMoving;

    auto adjustSizes(GameProperties &properties, Transform &transform, SSGE::CircleRendererComponent &rendererComponent)
        -> void;
};