#include "Renderer.h"
#include <stdexcept>
#include "../drivers/VulkanDriver.h"
#include "../utils/ShaderUtils.h"
#include "RectangleItem.h"

Renderer::Renderer(GLFWwindow *window, const RendererOptions& options) :
	m_window(window),
	m_options(options)
{
	initGraphicsDriver();
}

Renderer::~Renderer()
{
	for (auto item : m_items) {
		delete item;
	}

	m_driver->waitIdle();
	m_driver->cleanup();
}

void Renderer::render()
{
	updateRenderVertices();
	m_driver->drawFrame();
}

std::vector<Vertex> Renderer::getRectangleVertices(glm::vec2 topLeft, float width, float height, CoordinatesType coordinatesType, glm::vec3 fillColor)
{
	std::vector<Vertex> vertices;

	if (coordinatesType == CoordinatesType::Normalized) {
		vertices = {
			{topLeft, fillColor},
			{glm::vec2(topLeft.x + width, topLeft.y), fillColor},
			{glm::vec2(topLeft.x + width, topLeft.y + height), fillColor },
			{glm::vec2(topLeft.x, topLeft.y + height), fillColor}
		};
	}
	else {
		vertices = {
			{topLeft, fillColor},
			{glm::vec2(topLeft.x + width, topLeft.y), fillColor},
			{glm::vec2(topLeft.x + width, topLeft.y + height), fillColor },
			{glm::vec2(topLeft.x, topLeft.y + height), fillColor}
		};
	}

	return vertices;
}

RectangleItem* Renderer::createRectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor, CoordinatesType coordinatesType)
{
	RectangleItem* item = new RectangleItem(topLeft, width, height, fillColor, coordinatesType, this);
	m_items.push_back(item);
	return item;
}

void Renderer::initGraphicsDriver() {
	if (m_options.type == RendererOptions::Vulkan) {
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		m_driver = new VulkanDriver(getVulkanRequiredExtensions(), validationLayers, deviceExtensions, m_window,
			GraphicsDriverOptions{ m_options.debugModeOn, ShaderUtils::loadShader("vert.spv"), 	ShaderUtils::loadShader("frag.spv")
			});
	}
	else {
		throw std::runtime_error("Renderer type not supported");
	}

	m_driver->init();
}

std::vector<const char*> Renderer::getVulkanRequiredExtensions() const {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_options.debugModeOn) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void Renderer::updateRenderVertices() {
	if (m_changeQueue.empty())
		return;

	auto vertices = m_driver->getVertices();
	auto indices = m_driver->getIndices();

	for (auto item : m_changeQueue) {
		auto itemVertices = item->getVertices();
		auto itemIndices = item->getIndices();

		vertices.insert(vertices.end(), itemVertices.begin(), itemVertices.end());
		indices.insert(indices.end(), itemIndices.begin(), itemIndices.end());
	}

	m_driver->updateVertexBuffer(vertices);
	m_driver->updateIndexBuffer(indices);

	m_changeQueue.clear();
}
