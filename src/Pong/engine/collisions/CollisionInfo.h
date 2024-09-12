#pragma once

#include "glm/vec2.hpp"

class Collider;
struct CollisionInfo {
	Collider* collider;
	Collider* other;
	glm::vec2 contactPoint;
	bool entered;
};