#pragma once
#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include "../utils/Vertex.h"

struct GraphicsDriverOptions {
	bool debugModeOn;
	std::vector<char> vertexShader;
	std::vector<char> fragmentShader;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class GraphicsDriver
{
public:
	GraphicsDriver(GLFWwindow* window, const GraphicsDriverOptions& options) :
		m_window(window),
		m_options(options),
		m_framebufferResized(false),
		m_vertices()
	{

	}

	virtual void init() = 0;

	virtual void cleanup() = 0;

	virtual void drawFrame() = 0;

	virtual void waitIdle() = 0;

	virtual void updateVertexBuffer(const std::vector<Vertex>& newVertices) {
		m_vertices = newVertices;
	}

	virtual void updateIndexBuffer(const std::vector<uint16_t>& newIndices) {
		m_indices = newIndices;
	}

	void windowResized() {
		m_framebufferResized = true;
	}

	std::vector<Vertex> getVertices() {
		return m_vertices;
	}

	std::vector<uint16_t> getIndices() {
		return m_indices;
	}

protected:
	GLFWwindow* m_window;
	GraphicsDriverOptions m_options;
	bool m_framebufferResized;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto driver = reinterpret_cast<GraphicsDriver*>(glfwGetWindowUserPointer(window));
		driver->windowResized();
	}

	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;
};

