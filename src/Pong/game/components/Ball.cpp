#include "Ball.h"

#include "../../engine/collisions/CircleCollider.h"
#include "../../engine/graphics/renderers/scene/CircleItem.h"
#include "../../engine/scenes/Game.h"
#include "Racket.h"
#include "engine/scenes/components/CircleRendererComponent.h"

#include <ctime>
#include <iostream>

constexpr float BALL_RATIO = 0.03f;

Ball::Ball(SSGE::GameObject *gameObject)
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
    gameObject()->getTransform().translate({center, 0.0f});
    auto &circle = gameObject()->addComponent<SSGE::CircleRendererComponent>(gameObject());
    circle.setCenter({0.0f, 0.0f});
    circle.setRadius(m_radius);
    circle.setFillColor({1.0f, 1.0f, 0.0f, 1.0f});
    auto &collider = gameObject()->addComponent<SSGE::CircleCollider>(true, gameObject(), center, m_radius);
    collider.setLayer("ball");
    collider.setCollidesWith({"racket"});
    collider.addOnCollisionEnterCallback([this](const SSGE::CollisionInfo &info) { this->onCollisionEnter(info); });
    collider.addOnCollisionExitCallback([this](const SSGE::CollisionInfo &info) { this->onCollisionExit(info); });
}

void Ball::update()
{
    auto &rendererItem = *gameObject()->getComponent<SSGE::CircleRendererComponent>().value();
    auto &transform = gameObject()->getTransform();
    auto properties = Game::getInstance()->getProperties();
    adjustSizes(properties, transform, rendererItem);
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

void Ball::adjustSizes(GameProperties &properties, Transform &transform,
                       SSGE::CircleRendererComponent &rendererComponent)
{
    if (properties.width == m_lastWindowWidth && properties.height == m_lastWindowHeight)
        return;

    m_lastWindowWidth = properties.width;
    m_lastWindowHeight = properties.height;
    m_radius = BALL_RATIO * m_lastWindowHeight;
    rendererComponent.setRadius(m_radius);

    if (!m_isMoving)
    {
        auto center = glm::vec2(m_lastWindowWidth / 2.0f - transform.position().x,
                                m_lastWindowHeight / 2.0f - transform.position().y);
        transform.translate({center, 0.0f});
    }
}