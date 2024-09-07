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

class GraphicsOperation;
class GraphicsDriver
{
public:
	GraphicsDriver(GLFWwindow* window, const GraphicsDriverOptions& options) :
		m_window(window),
		m_options(options),
		m_framebufferResized(false)
	{

	}

	virtual void init() = 0;

	virtual void cleanup() = 0;

	virtual void drawFrame(const std::vector<GraphicsOperation*>& updateOperations) = 0;

	virtual void performOperation(GraphicsOperation* operation) = 0;

	virtual void waitIdle() = 0;

	void windowResized() {
		m_framebufferResized = true;
	}

	virtual glm::vec2 getWindowSize() const = 0;

protected:
	GLFWwindow* m_window;
	GraphicsDriverOptions m_options;
	bool m_framebufferResized;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto driver = reinterpret_cast<GraphicsDriver*>(glfwGetWindowUserPointer(window));
		driver->windowResized();
	}
};

