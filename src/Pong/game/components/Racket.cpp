#include "Racket.h"
#include "../../engine/collisions/QuadCollider.h"
#include "../../engine/graphics/renderers/scene/RectangleItem.h"
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
    glm::vec2 topLeft;
    if (m_left)
    {
        topLeft = {10.0f, middle};
        auto &quad = gameObject().addComponent<SSGE::QuadRendererComponent>(gameObject());
        quad.setTopLeft(topLeft);
        quad.setWidth(m_width);
        quad.setHeight(m_height);
        quad.setFillColor({0.0f, 1.0f, 0.0f, 1.0f});
    }
    else
    {
        topLeft = {m_lastWindowWidth - m_width - 10.0f, middle};
        auto &quad = gameObject().addComponent<SSGE::QuadRendererComponent>(gameObject());
        quad.setTopLeft(topLeft);
        quad.setWidth(m_width);
        quad.setHeight(m_height);
        quad.setFillColor({1.0f, 0.0f, 0.0f, 1.0f});
    }
    auto &collider = gameObject().addComponent<SSGE::QuadCollider>(false, gameObject(), topLeft, m_width, m_height);
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
    auto &rendererItem = *gameObject().getComponent<SSGE::QuadRendererComponent>().value();
    auto collider = *gameObject().getComponent<SSGE::QuadCollider>().value();
    auto properties = Game::getInstance()->getProperties();
    adjustSizes(properties, rendererItem);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_lastTime).count();

    constexpr float speed = 2.0f;

    if (m_direction == -1)
        m_currentStep = 1;
    else if (m_direction == 1)
        m_currentStep = 0;
    else
        m_currentStep = 255;

    // if (duration > 5 && m_currentStep <= 1)
    // {
    //     m_lastTime = std::chrono::high_resolution_clock::now();
    //     if (rendererItem->getTopLeft().y + m_steps[m_currentStep] > m_topLimit &&
    //         rendererItem->getTopLeft().y + m_steps[m_currentStep] < m_bottomLimit)
    //     {
    //         rendererItem->moveY(m_steps[m_currentStep] * speed);
    //         collider->setTopLeft(rendererItem->getTopLeft());
    //     }
    // }
}

void Racket::adjustSizes(GameProperties &properties, SSGE::QuadRendererComponent &rendererItem)
{
    if (properties.width != m_lastWindowWidth || properties.height != m_lastWindowHeight)
    {
        m_lastWindowWidth = properties.width;
        m_lastWindowHeight = properties.height;
        m_height = 0.15f * m_lastWindowHeight;
        m_bottomLimit = m_lastWindowHeight - m_height - 5.0f;
        auto middle = (m_lastWindowHeight - m_height) / 2;
        rendererItem.setHeight(m_height);
        if (!m_left)
        {
            auto rightRacketPos = m_lastWindowWidth - m_width - 10.0f;
            //rendererItem.moveXTo(rightRacketPos);
        }
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
    if (m_left && ((key == GLFW_KEY_W && m_direction == 1) || (key == GLFW_KEY_S && m_direction == -1)))
        m_direction = 0;
    else if (!m_left && ((key == GLFW_KEY_UP && m_direction == 1) || (key == GLFW_KEY_DOWN && m_direction == -1)))
        m_direction = 0;
}