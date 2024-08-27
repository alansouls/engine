#pragma once
#include <optional>
#include <vector>

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
		m_options(options)
	{

	}

	virtual void init() = 0;

	virtual void cleanup() = 0;

protected:
	GLFWwindow* m_window;
	GraphicsDriverOptions m_options;
};

