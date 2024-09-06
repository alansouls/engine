#pragma once
#include "RendererItem.h"
#include "Renderer.h"

class RectangleItem :
    public RendererItem
{
public:
	RectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor, Renderer::CoordinatesType coordinatesType, Renderer* renderer);
	~RectangleItem() {}

	void setRectangle(glm::vec2 topLeft, float width, float height) {
		m_topLeft = topLeft;
		m_width = width;
		m_height = height;

		updateGeometry();
		updateTransform();
	}

	void updateGeometry() override {
		auto vertices = m_renderer->getRectangleVertices(m_topLeft, m_width, m_height, m_coordinatesType, getFillColor());
		geometryUpdated(vertices, { 0, 1, 2, 2, 3, 0 });
	}

	void updateTransform() override {
		setTransform(m_renderer->getTransformPosition(m_topLeft, m_coordinatesType), getTransformScale());
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

private:
	glm::vec2 m_topLeft;
	float m_width;
	float m_height;
	Renderer::CoordinatesType m_coordinatesType;
};

