#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

struct CircleVertex {
	glm::vec2 pos;
	glm::vec3 color;
	float radius;
};