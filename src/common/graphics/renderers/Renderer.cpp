#include "Renderer.h"

#include <algorithm>
#include <stdexcept>
#include "../drivers/VulkanDriver.h"
#include "../utils/ShaderUtils.h"
#include "RectangleItem.h"

Renderer::Renderer(GLFWwindow *window, const RendererOptions& options) :
	m_window(window),
	m_options(options)
{
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, Renderer::framebufferResizeCallback);
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

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->m_driver->windowResized();
		
	for (auto item : renderer->m_items) {
		item->updateGeometry();
		renderer->m_changeSet.insert(item);
	}
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
		int wWidth; 
		int wHeight;
		glfwGetWindowSize(m_window, &wWidth, &wHeight);
		glm::vec2 normalizedTopLeft = { (topLeft.x / wWidth * 2.0f) - 1.0f, (topLeft.y / wHeight * 2.0f) - 1.0f };
		float normalizedWidth = (width / wWidth * 2.0f);
		float normalizedHeight = (height / wHeight * 2.0f);
		vertices = {
			{normalizedTopLeft, fillColor},
			{glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y), fillColor},
			{glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y + normalizedHeight), fillColor },
			{glm::vec2(normalizedTopLeft.x, normalizedTopLeft.y + normalizedHeight), fillColor}
		};
	}

	return vertices;
}

RectangleItem* Renderer::createRectangleItem(glm::vec2 topLeft, float width, float height, glm::vec3 fillColor, CoordinatesType coordinatesType)
{
	RectangleItem* item = new RectangleItem(topLeft, width, height, fillColor, coordinatesType, this);
	m_items.push_back(item);
	m_changeSet.insert(item);
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

	return extensions;
}

void Renderer::updateRenderVertices() {
	if (m_changeSet.empty())
		return;

	auto vertices = m_driver->getVertices();
	auto indices = m_driver->getIndices();

	size_t verticesOffset = 0;
	size_t indicesOffset = 0;
	for (auto item : m_items) {
		auto itemVertices = item->getVertices();
		auto itemIndices = item->getIndices();
		adjustIndices(itemIndices, indicesOffset);
		if (m_changeSet.contains(item)) {
			//in case we have to add
			if (itemVertices.size() + verticesOffset > vertices.size()) {
				vertices.insert(vertices.end(), itemVertices.begin(), itemVertices.end());
				indices.insert(indices.end(), itemIndices.begin(), itemIndices.end());
			}
			else {
				std::copy(itemVertices.begin(), itemVertices.end(), vertices.begin() + verticesOffset);
				std::copy(itemIndices.begin(), itemIndices.end(), indices.begin() + indicesOffset);
			}
		}
		verticesOffset += itemVertices.size();
		indicesOffset += itemIndices.size();
	}

	m_driver->updateVertexBuffer(vertices);
	m_driver->updateIndexBuffer(indices);

	m_changeSet.clear();
}

void Renderer::adjustIndices(std::vector<uint16_t>& indices, size_t verticeOffset) {
	for (size_t i = 0; i < indices.size(); i++)
		indices[i] += (uint16_t)verticeOffset;
}