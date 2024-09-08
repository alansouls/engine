#include "Racket.h"
#include "../../engine/graphics/renderers/RectangleItem.h"
#include <GLFW/glfw3.h>
#include "../../engine/scenes/GameObject.h"

Racket::Racket(bool left) : GameObject(),
m_left(left),
m_lastTime(),
m_originalWindowWidth(0.0f),
m_originalWindowHeight(0.0f),
m_lastWindowWidth(0.0f),
m_lastWindowHeight(0.0f),
m_width(0.0f),
m_height(0.0f),
m_bottomLimit(0.0f),
m_currentStep(0),
m_direction(0)
{
}

void Racket::init()
{
	auto properties = getGameProperties();
	m_originalWindowWidth = properties.width;
	m_originalWindowHeight = properties.height;
	m_lastWindowWidth = m_originalWindowWidth;
	m_lastWindowHeight = m_originalWindowHeight;
	m_width = 20.0f;
	m_height = 0.15f * m_lastWindowHeight;
	float middle = (m_lastWindowHeight - m_height) / 2;
	if (m_left) {
		auto racket = new RectangleItem({ 10.0f, middle }, m_width, m_height, { 0.0f, 1.0f, 0.0f });
		setRendererItem(racket);
	}
	else {
		auto racket = new RectangleItem({ m_lastWindowWidth - m_width - 10.0f, middle }, m_width, m_height, { 1.0f, 0.0f, 0.0f });
		setRendererItem(racket);
	}
	m_lastTime = std::chrono::high_resolution_clock::now();
	if (m_left)
		m_currentStep = 0;
	else
		m_currentStep = 1;
	m_bottomLimit = properties.height - m_height - 5.0f;
}

void Racket::update()
{
	auto rendererItem = reinterpret_cast<RectangleItem*>(getRendererItem());
	auto properties = getGameProperties();
	adjustSizes(properties, rendererItem);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_lastTime).count();

	const float speed = 2.0f;

	if (m_direction == -1)
		m_currentStep = 1;
	else if (m_direction == 1)
		m_currentStep = 0;
	else
		m_currentStep = -1;

	if (duration > 5 && m_currentStep >= 0) {
		m_lastTime = std::chrono::high_resolution_clock::now();
		if (rendererItem->getTopLeft().y + m_steps[m_currentStep] > m_topLimit && rendererItem->getTopLeft().y + m_steps[m_currentStep] < m_bottomLimit) {
			rendererItem->moveY(m_steps[m_currentStep] * speed);
		}
	}
}

void Racket::adjustSizes(GameProperties& properties, RectangleItem* rendererItem)
{
	if (properties.width != m_lastWindowWidth || properties.height != m_lastWindowHeight) {
		m_lastWindowWidth = properties.width;
		m_lastWindowHeight = properties.height;
		m_height = 0.15f * m_lastWindowHeight;
		m_bottomLimit = m_lastWindowHeight - m_height - 5.0f;
		auto middle = (m_lastWindowHeight - m_height) / 2;
		rendererItem->setScale({ 1.0f, m_lastWindowHeight / m_originalWindowHeight, 1.0f });
		if (!m_left) {
			auto rightRacketPos = m_lastWindowWidth - m_width - 10.0f;
			rendererItem->moveXTo(rightRacketPos);
		}

		if (rendererItem->getTopLeft().y > m_bottomLimit) {
			rendererItem->moveY(m_bottomLimit - rendererItem->getTopLeft().y);
		}
	}
}

void Racket::onKeyPressed(int key)
{
	if (m_left) {
		if (key == GLFW_KEY_W) {
			m_direction = 1;
		}
		else if (key == GLFW_KEY_S) {
			m_direction = -1;
		}
	}
	else {
		if (key == GLFW_KEY_UP) {
			m_direction = 1;
		}
		else if (key == GLFW_KEY_DOWN) {
			m_direction = -1;
		}
	}
}

void Racket::onKeyReleased(int key)
{
	if (m_left && ((key == GLFW_KEY_W && m_direction == 1) || (key == GLFW_KEY_S && m_direction == -1)))
		m_direction = 0;
	else if ((key == GLFW_KEY_UP && m_direction == 1) || (key == GLFW_KEY_DOWN && m_direction == -1))
		m_direction = 0;
}