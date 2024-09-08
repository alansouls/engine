#include "RendererItem.h"

class CircleItem : public RendererItem {
public:
	CircleItem(glm::vec2 center, float radius, glm::vec3 fillColor);
	~CircleItem() {};

	void setCircle(glm::vec2 center, float radius) {
		m_center = center;
		m_radius = radius;

		updateGeometry();
		updateTransform();
	}

	void updateGeometry() override;

	void updateTransform() override {
		setTransform(glm::vec3(m_center.x, m_center.y, 0.0f), getTransformScale());
	}

	glm::vec2 getCenter() const {
		return m_center;
	}

	void moveTo(glm::vec2 center) {
		setCircle(center, m_radius);
	}

	void move(float step, glm::vec2 direction) {
		setCircle({ m_center.x + direction.x * step, m_center.y + direction.y * step }, m_radius);
	}

	float getRadius() const {
		return m_radius;
	}

	void setRadius(float radius) {
		setCircle(m_center, radius);
	}

	glm::vec3 getFillColor() const;
	void setFillColor(const glm::vec3& fillColor);

private:
	glm::vec2 m_center;
	float m_radius;
	glm::vec3 m_fillColor;
};