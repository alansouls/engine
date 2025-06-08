#pragma once
#include "../../engine/scenes/GameObject.h"
#include <glm/vec2.hpp>

class CircleCollider;
class CircleItem;
class Ball : public SSGE::Component
{
  public:
    Ball();

    void init() override;

    void update() override;

    void onKeyReleased(int key);

    void onCollisionEnter(const CollisionInfo &info);
    void onCollisionExit(const CollisionInfo &info);

  private:
    float m_originalWindowWidth;
    float m_originalWindowHeight;

    float m_lastWindowWidth;
    float m_lastWindowHeight;

    float m_radius;

    glm::vec2 m_direction;

    bool m_isMoving;

    void adjustSizes(GameProperties &properties, CircleItem *rendererItem, CircleCollider *collider);
};