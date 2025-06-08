#include "Ball.h"

#include "../../engine/collisions/CircleCollider.h"
#include "../../engine/graphics/renderers/scene/CircleItem.h"
#include "../../engine/scenes/Game.h"
#include "Racket.h"
#include "engine/scenes/components/CircleRendererComponent.h"

#include <ctime>
#include <iostream>

constexpr float BALL_RATIO = 0.03f;

Ball::Ball(SSGE::GameObject* gameObject)
    : Component("Ball", gameObject), m_originalWindowWidth(0), m_originalWindowHeight(0), m_lastWindowWidth(0),
      m_lastWindowHeight(0), m_radius(0), m_direction(), m_isMoving(false)
{
}

void Ball::init()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    m_isMoving = false;
    auto properties = Game::getInstance()->getProperties();
    m_originalWindowWidth = static_cast<float>(properties.width);
    m_originalWindowHeight = static_cast<float>(properties.height);
    m_lastWindowWidth = m_originalWindowWidth;
    m_lastWindowHeight = m_originalWindowHeight;
    m_radius = BALL_RATIO * m_lastWindowHeight;

    glm::vec2 center = {m_lastWindowWidth / 2.0f, m_lastWindowHeight / 2.0f};
    // setRendererItem(new CircleItem(center, m_radius, {1.0f, 1.0f, 0.0f}));
    // auto collider = new CircleCollider(true, this, center, m_radius);
    // collider->setLayer("ball");
    // collider->setCollidesWith({"racket"});
    // setCollider(collider);

    auto &collider = *gameObject().getComponent<SSGE::CircleCollider>().value();
    collider.addOnCollisionEnterCallback([this](const SSGE::CollisionInfo& info){ this->onCollisionEnter(info);});
    collider.addOnCollisionExitCallback([this](const SSGE::CollisionInfo& info){ this->onCollisionExit(info);});
}

void Ball::update()
{
}

void Ball::onKeyReleased(int key)
{
    if (key == GLFW_KEY_SPACE && !m_isMoving)
    {
        m_isMoving = true;
        float xDir = (rand() % 2);
        float yDir = (rand() % 10000) / 10000.0f;

        if (xDir == 1)
        {
            xDir = 1.0f;
            yDir *= -1.0f;
        }
        else
            xDir = -1.0f;

        m_direction = {xDir, yDir};
    }
}

void Ball::onCollisionEnter(const SSGE::CollisionInfo &info)
{
    m_direction.x = -m_direction.x;
}

void Ball::onCollisionExit(const SSGE::CollisionInfo &info)
{
}

void Ball::adjustSizes(GameProperties &properties, SSGE::CircleRendererComponent *rendererComponent,
                       SSGE::CircleCollider *collider)
{
    if (properties.width == m_lastWindowWidth && properties.height == m_lastWindowHeight)
        return;

    m_lastWindowWidth = properties.width;
    m_lastWindowHeight = properties.height;
    m_radius = BALL_RATIO * m_lastWindowHeight;
    rendererComponent->setRadius(m_radius);
    collider->setRadius(m_radius);
    if (!m_isMoving)
        rendererComponent->setCenter({m_lastWindowWidth / 2.0f, m_lastWindowHeight / 2.0f});
}