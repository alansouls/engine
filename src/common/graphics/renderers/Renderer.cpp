#include "Renderer.h"

#include <vector>
#include <algorithm>
#include <stdexcept>
#include "../drivers/VulkanDriver.h"
#include "../utils/ShaderUtils.h"
#include "RectangleItem.h"
#include <map>
#include <vector>

Renderer::Renderer(GLFWwindow *window, const RendererOptions& options) :
	m_window(window),
	m_options(options)
{
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, Renderer::framebufferResizeCallback);
    setDimensions();
	initGraphicsDriver();
}

Renderer::~Renderer()
{
	for (auto item : m_addedSet) {
		delete item;
	}

	for (auto item : m_items) {
		delete item.second;
	}

	m_driver->waitIdle();
	m_driver->cleanup();
}

void Renderer::render()
{
	auto addOrRemoveOperations = getAddOrRemoveOperations();

	if (!addOrRemoveOperations.empty())
		m_driver->waitIdle();

	for (auto item : addOrRemoveOperations) {
		auto operation = item.second;
		auto renderItem = item.first;
		m_driver->performOperation(&operation);

		if (!operation.result.has_value() || operation.result.value() < 0)
			continue;

		if (operation.type == GraphicsOperation::Type::Add)
		{
			renderItem->setKey(operation.result.value());
			m_items.insert(std::make_pair(operation.result.value(), renderItem));
		}
		else if (operation.type == GraphicsOperation::Type::Remove)
		{
			m_items.erase(operation.key);
		}
	}

	std::vector<GraphicsOperation> updateOperations = getUpdateOperations();
	std::vector<GraphicsOperation *> operations;
	for (auto& operation : updateOperations) {
		operations.push_back(&operation);
	}
	m_driver->drawFrame(operations);
}

void Renderer::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    renderer->setDimensions();
	renderer->m_driver->windowResized();

	for (auto item : renderer->m_items) {
		item.second->updateTransform();
		renderer->m_updatedSet.insert(item.first);
	}
}

void Renderer::setDimensions() {
    int wWidth;
    int wHeight;
    glfwGetWindowSize(m_window, &wWidth, &wHeight);
    m_width = wWidth;
    m_height = wHeight;
}

glm::vec3 Renderer::getResizeScale(float width, float height) const
{
    int wWidth;
    int wHeight;
    glfwGetWindowSize(m_window, &wWidth, &wHeight);
    glm::vec2 normalizedSize = { width / wWidth * 2, height / wHeight * 2 };
    return glm::vec3(normalizedSize, 1.0f);
}

void Renderer::addItem(RendererItem* item)
{
    item->addCallback(this, &itemUpdated);
	m_addedSet.insert(item);
}

void Renderer::itemUpdated(void* thisPtr, uint32_t itemKey)
{
    auto renderer = reinterpret_cast<Renderer *>(thisPtr);
    renderer->m_updatedSet.insert(itemKey);
}

void Renderer::initGraphicsDriver() {
	if (m_options.type == RendererOptions::Vulkan) {
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation",
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef MACOSX
            "VK_KHR_portability_subset"
#endif
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

std::map<RendererItem *, GraphicsOperation> Renderer::getAddOrRemoveOperations()
{
	std::map<RendererItem*, GraphicsOperation> addOrRemoveOperations;
	if (m_addedSet.empty() && m_removedSet.empty())
		return addOrRemoveOperations;

	for (auto added : m_addedSet) {
		GraphicsOperation operation;
		operation.type = GraphicsOperation::Type::Add;
		operation.vertices = added->getVertices();
		operation.indices = added->getIndices();
		operation.transformPosition = added->getTransformPosition();
		operation.transformScale = added->getTransformScale();
		addOrRemoveOperations.insert(std::make_pair(added, operation));
	}

	for (auto removed : m_removedSet) {
		GraphicsOperation operation;
		operation.type = GraphicsOperation::Type::Remove;
		operation.key = removed;
		addOrRemoveOperations.insert(std::make_pair(nullptr, operation));
	}

	m_addedSet.clear();
	m_removedSet.clear();

	return addOrRemoveOperations;
}

std::vector<GraphicsOperation> Renderer::getUpdateOperations()
{
	std::vector<GraphicsOperation> updateOperations;

	if (m_updatedSet.empty())
		return updateOperations;

	for (auto key : m_updatedSet) {
		auto updated = m_items.find(key)->second;
		if (updated == nullptr)
			continue;

		GraphicsOperation operation;
		operation.type = GraphicsOperation::Type::Update;
		operation.transformPosition = updated->getTransformPosition();
		operation.transformScale = updated->getTransformScale();
		operation.key = key;
		updateOperations.push_back(operation);
	}

	m_updatedSet.clear();

	return updateOperations;

}
