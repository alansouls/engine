#pragma once
#include "RendererItem.h"
#include "SceneRenderer.h"

class RectangleItem : public RendererItem
{
  public:
    RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec4 fillColor);
    ~RectangleItem() override = default;

    void setRectangle(glm::vec2 topLeft, float width, float height)
    {
        m_topLeft = topLeft;
        m_width = width;
        m_height = height;

        updateTransform();
    }

    void updateTransform() override
    {
        setTransform(glm::vec3(m_topLeft.x, m_topLeft.y, 0.0f), glm::vec3(m_width, m_height, 1.0f));
    }

    [[nodiscard]] auto getTopLeft() const -> glm::vec2;

    void moveY(float step);

    auto moveXTo(float newX) -> void;

    [[nodiscard]] auto getWidth() const -> float;

    auto setHeight(float height) -> void;

    auto setWidth(float width) -> void;

    [[nodiscard]] float getHeight() const;

    [[nodiscard]] auto getFillColor() const -> glm::vec4;
    auto setFillColor(const glm::vec4 &fillColor) -> void;

  private:
    glm::vec2 m_topLeft;
    float m_width;
    float m_height;
    glm::vec4 m_fillColor;
};
