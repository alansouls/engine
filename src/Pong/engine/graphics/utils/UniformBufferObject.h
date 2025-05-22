#pragma once

#include <glm/mat4x4.hpp>

struct InstanceData {
	glm::mat4 model;
	alignas(16) glm::vec3 inColor;
};

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
};