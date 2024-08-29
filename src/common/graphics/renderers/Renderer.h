#pragma once

#include <optional>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "../utils/Vertex.h"

struct RendererOptions {
	enum RendererType {
		Vulkan,
		RendererTypeCount
	};

	RendererType type;
	bool debugModeOn;
	std::optional<uint32_t> fpsCap;
};

struct GLFWwindow;
class GraphicsDriver;
class RectangleItem;
class RendererItem;
class Renderer
{
public:
	enum CoordinatesType {
		Normalized,
		Pixel
	};

	Renderer(GLFWwindow* window, const RendererOptions& options);
	~Renderer();

	void render();

	std::vector<Vertex> getRectangleVertices(glm::vec2 topLeft, float width, float height, CoordinatesType coordinatesType, glm::vec3 fillColor);

	RectangleItem* createRectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor, CoordinatesType coordinatesType);

private:
	GLFWwindow* m_window;
	RendererOptions m_options;

	GraphicsDriver* m_driver;

	std::vector<RendererItem*> m_items;

	std::vector<RendererItem*> m_changeQueue;

	void initGraphicsDriver();

	std::vector<const char*> getVulkanRequiredExtensions() const;

	void updateRenderVertices();

	friend class RendererItem;
};

