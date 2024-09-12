#include "Ball.h"
#include "../../engine/scenes/Game.h"
#include "../../engine/graphics/renderers/CircleItem.h"
#include <ctime>
#include <iostream>
#include "../../engine/collisions/CircleCollider.h"

Ball::Ball() : GameObject()
{
}

void Ball::init()
{
	srand(time(0));
	m_isMoving = false;
	auto properties = Game::getInstance()->getProperties();
	m_originalWindowWidth = properties.width;
	m_originalWindowHeight = properties.height;
	m_lastWindowWidth = m_originalWindowWidth;
	m_lastWindowHeight = m_originalWindowHeight;
	m_radius = 0.015f * m_lastWindowHeight;

	glm::vec2 center = { m_lastWindowWidth / 2.0f, m_lastWindowHeight / 2.0f };
	setRendererItem(new CircleItem(center, m_radius, { 1.0f, 1.0f, 0.0f }));
	setCollider(new CircleCollider(true, this, center, m_radius));

	m_lastTime = std::chrono::high_resolution_clock::now();
}

void Ball::update()
{
	auto rendererItem = reinterpret_cast<CircleItem *>(getRendererItem());
	auto collider = reinterpret_cast<CircleCollider*>(getCollider());
	auto properties = getGameProperties();
	adjustSizes(properties, rendererItem, collider);

	if (!m_isMoving)
		return;

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_lastTime).count();

	float speed = 0.004f * m_lastWindowWidth;
	if (duration > 10) {
		m_lastTime = stop;
		auto position = rendererItem->getTransformPosition();
		if (position.x - m_radius < 0.0f || position.x + m_radius > m_lastWindowWidth) {
			m_isMoving = false;
			glm::vec2 center = { m_lastWindowWidth / 2.0f, m_lastWindowHeight / 2.0f };
			position = glm::vec3(center, 0.0f);
			rendererItem->setPosition(position);
			collider->setCenter(position);
			return;
		}
		if (position.y - m_radius < 0.0f || position.y + m_radius > m_lastWindowHeight) {
			m_direction.y = -m_direction.y;
		}
		position.x += m_direction.x * speed;
		position.y += m_direction.y * speed;
		rendererItem->setPosition(position);
		collider->setCenter(position);
	}
}

void Ball::onKeyReleased(int key)
{
	if (key == GLFW_KEY_SPACE && !m_isMoving) {
		m_isMoving = true;
		float xDir = (rand() % 10000) / 10000.0f;
		float yDir = (rand() % 10000) / 10000.0f;
		m_direction = { -1.0, yDir };
	}
}

void Ball::onCollisionEnter(const CollisionInfo& info)
{
	m_direction.x = -m_direction.x;
}

void Ball::onCollisionExit(const CollisionInfo& info)
{
}

void Ball::adjustSizes(GameProperties& properties, CircleItem* rendererItem, CircleCollider* collider)
{
	if (properties.width != m_lastWindowWidth || properties.height != m_lastWindowHeight) {
		m_lastWindowWidth = properties.width;
		m_lastWindowHeight = properties.height;
		m_radius = 0.015f * m_lastWindowHeight;
		rendererItem->setScale({ m_lastWindowHeight / m_originalWindowHeight, m_lastWindowHeight / m_originalWindowHeight, 1.0f });
		collider->setRadius(m_radius);
		if (!m_isMoving)
			rendererItem->setPosition({ m_lastWindowWidth / 2.0f, m_lastWindowHeight / 2.0f, 0.0f });
	}
}
