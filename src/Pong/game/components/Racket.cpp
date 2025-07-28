#include "Racket.h"
#include "../../../engine/collisions/QuadCollider.h"
#include "../../../engine/graphics/renderers/scene/RectangleItem.h"
#include "engine/scenes/Game.h"
#include "engine/scenes/components/QuadRendererComponent.h"

#include <GLFW/glfw3.h>

Racket::Racket(SSGE::GameObject *gameObject, bool left)
    : Component("RacketComponent", gameObject), m_left(left), m_lastTime(), m_originalWindowWidth(0.0f),
      m_originalWindowHeight(0.0f), m_lastWindowWidth(0.0f), m_lastWindowHeight(0.0f), m_width(0.0f), m_height(0.0f),
      m_bottomLimit(0.0f), m_currentStep(0), m_direction(0)
{
}

void Racket::init()
{
    const auto properties = Game::getInstance()->getProperties();
    m_originalWindowWidth = static_cast<float>(properties.width);
    m_originalWindowHeight = static_cast<float>(properties.height);
    m_lastWindowWidth = m_originalWindowWidth;
    m_lastWindowHeight = m_originalWindowHeight;
    m_width = 50.0f;
    m_height = 0.15f * m_lastWindowHeight;
    float middle = (m_lastWindowHeight - m_height) / 2;
    glm::vec2 topLeft = {0.0f, 0.0f};
    if (m_left)
    {
        auto &quad = gameObject()->addComponent<SSGE::QuadRendererComponent>(gameObject());
        gameObject()->getTransform().translate({10.0f, middle, 0.0f});
        quad.setWidth(m_width);
        quad.setHeight(m_height);
        quad.setFillColor({0.0f, 1.0f, 0.0f, 1.0f});
    }
    else
    {
        auto &quad = gameObject()->addComponent<SSGE::QuadRendererComponent>(gameObject());
        gameObject()->getTransform().translate({m_lastWindowWidth - m_width - 10.0f, middle, 0.0f});
        quad.setWidth(m_width);
        quad.setHeight(m_height);
        quad.setFillColor({1.0f, 0.0f, 0.0f, 1.0f});
    }
    auto &collider = gameObject()->addComponent<SSGE::QuadCollider>(false, gameObject(), topLeft, m_width, m_height);
    collider.setLayer("racket");
    m_lastTime = std::chrono::high_resolution_clock::now();
    if (m_left)
        m_currentStep = 0;
    else
        m_currentStep = 1;
    m_bottomLimit = properties.height - m_height - 5.0f;
}

void Racket::update()
{
    auto &rendererItem = *gameObject()->getComponent<SSGE::QuadRendererComponent>().value();
    auto collider = *gameObject()->getComponent<SSGE::QuadCollider>().value();
    auto &transform = gameObject()->getTransform();
    auto properties = Game::getInstance()->getProperties();
    adjustSizes(properties, transform, rendererItem);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_lastTime).count();

    constexpr float speed = 2.0f;

    if (m_direction == -1)
        m_currentStep = 1;
    else if (m_direction == 1)
        m_currentStep = 0;
    else
        m_currentStep = 2;

    if (duration > 5 && m_currentStep <= 1)
    {
        m_lastTime = std::chrono::high_resolution_clock::now();
        auto position = transform.position();
        if (position.y + m_steps[m_currentStep] > m_topLimit && position.y + m_steps[m_currentStep] < m_bottomLimit)
        {
            transform.translate(glm::vec3(0.0f, m_steps[m_currentStep] * speed, 0.0f));
        }
    }
}

void Racket::adjustSizes(GameProperties &properties, Transform &transform,
                         SSGE::QuadRendererComponent &rendererComponent)
{
    if (properties.width == m_lastWindowWidth && properties.height == m_lastWindowHeight)
        return;

    m_lastWindowWidth = properties.width;
    m_lastWindowHeight = properties.height;
    m_height = 0.15f * m_lastWindowHeight;
    m_bottomLimit = m_lastWindowHeight - m_height - 5.0f;
    rendererComponent.setHeight(m_height);
    if (!m_left)
    {
        auto rightRacketPos = m_lastWindowWidth - m_width - 10.0f;
        transform.translate({rightRacketPos - transform.position().x, 0.0f, 0.0f});
    }
}

void Racket::onKeyPressed(int key)
{
    if (m_left)
    {
        if (key == GLFW_KEY_W)
        {
            m_direction = 1;
        }
        else if (key == GLFW_KEY_S)
        {
            m_direction = -1;
        }
    }
    else
    {
        if (key == GLFW_KEY_UP)
        {
            m_direction = 1;
        }
        else if (key == GLFW_KEY_DOWN)
        {
            m_direction = -1;
        }
    }
}

void Racket::onKeyReleased(int key)
{
    auto upKey = m_left ? GLFW_KEY_W : GLFW_KEY_UP;
    auto downKey = m_left ? GLFW_KEY_S : GLFW_KEY_DOWN;
    if ((key == upKey && m_direction == 1) || (key == downKey && m_direction == -1))
        m_direction = 0;
}