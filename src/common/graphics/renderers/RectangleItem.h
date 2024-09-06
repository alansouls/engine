#pragma once
#include "RendererItem.h"
#include "Renderer.h"

class RectangleItem :
    public RendererItem
{
public:
    RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor);
	~RectangleItem() {}

	void setRectangle(glm::vec2 topLeft, float width, float height) {
		m_topLeft = topLeft;
		m_width = width;
		m_height = height;

		updateGeometry();
		updateTransform();
	}

    void updateGeometry() override;

	void updateTransform() override {
		setTransform(glm::vec3(m_topLeft.x, m_topLeft.y, 0.0f), getTransformScale());
	}

	glm::vec2 getTopLeft() const {
		return m_topLeft;
	}
    
    void moveY(float step) {
        setRectangle({m_topLeft.x, m_topLeft.y + step }, m_width, m_height);
    }

	void moveXTo(float newX) {
		setRectangle({ newX, m_topLeft.y }, m_width, m_height);
	}

	float getWidth() const {
		return m_width;
	}

	float getHeight() const {
		return m_height;
	}
    
    glm::vec3 getFillColor() const;
    void setFillColor(const glm::vec3& fillColor);

private:
	glm::vec2 m_topLeft;
	float m_width;
	float m_height;
    glm::vec3 m_fillColor;
};

