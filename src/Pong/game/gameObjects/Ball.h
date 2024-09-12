#pragma once
#include "../../engine/scenes/GameObject.h"
#include <chrono>
#include <glm/vec2.hpp>

class CircleCollider;
class CircleItem;
class Ball : public GameObject
{
public:
	Ball();

	void init() override;

	void update() override;

	void onKeyReleased(int key) override;

	void onCollisionEnter(const CollisionInfo& info) override;
	void onCollisionExit(const CollisionInfo& info) override;
private:
	std::chrono::high_resolution_clock::time_point m_lastTime;

	float m_originalWindowWidth;
	float m_originalWindowHeight;

	float m_lastWindowWidth;
	float m_lastWindowHeight;

	float m_radius;

	glm::vec2 m_direction;

	bool m_isMoving;

	void adjustSizes(GameProperties& properties, CircleItem* rendererItem, CircleCollider* collider);
};