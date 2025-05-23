#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <set>
#include "GraphicsDriver.h"

const int MAX_FRAMES_IN_FLIGHT = 2;
const int MAX_INSTANCES = 10000;

struct InstanceData;

struct PrimitiveData 
{
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkPipeline* graphicsPipeline;
	size_t indicesSize;
};

struct Camera {
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSet;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
};

struct GraphicElement {
	GraphicsDriver::ElementType type;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<VkBuffer> storageBuffers;
	std::vector<VkDeviceMemory> storageBuffersMemory;
	std::vector<void*> storageBuffersMapped;

	std::vector<InstanceData> instanceData;
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};
class VulkanDriver : public GraphicsDriver
{
public:
	VulkanDriver(const std::vector<const char*> requiredExtensions, const std::vector<const char*>& validationLayers, const std::vector<const char*>& deviceExtensions,
		GLFWwindow* window, const GraphicsDriverOptions& options);

	void init() override;

	void cleanup() override;

	void drawFrame(const std::vector<GraphicsOperation*>& updateOperations) override;

	void performOperation(GraphicsOperation* operation) override;

	void waitIdle() override;

	glm::vec2 getWindowSize() const override {
		return { m_swapChainExtent.width, m_swapChainExtent.height };
	}

private:
	Camera m_camera;

	std::map<ElementType, PrimitiveData> m_primitives;

	std::vector<const char*> m_validationLayers;
	std::vector<const char*> m_deviceExtensions;
	std::vector<const char*> m_requiredExtensions;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkSurfaceKHR m_surface;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_logicalDevice;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkSwapchainKHR m_swapChain;

	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	std::vector<VkImageView> m_swapChainImageViews;

	VkRenderPass m_renderPass;
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkPipelineLayout m_defaultPipelineLayout;
	VkPipelineLayout m_circlePipelineLayout;

	VkPipeline m_defaultGraphicsPipeline;
	VkPipeline m_circleGraphicsPipeline;

	std::vector<VkFramebuffer> m_swapChainFramebuffers;

	VkCommandPool m_commandPool;

	std::vector<VkCommandBuffer> m_commandBuffers;

	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;

	uint32_t m_currentFrame = 0;

	std::map<ElementType, std::vector<GraphicElement*>> m_elementsByType;

	float m_extentFactorWidth;
	float m_extentFactorHeight;

	void updateVertexBuffer(GraphicElement* element);
	void updateIndexBuffer(GraphicElement* element);

	void createInstance();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void setupDebugMessenger();

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void pickPhysicalDevice();

	bool isDeviceSuitable(VkPhysicalDevice device);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void createLogicalDevice();

	void createSurface();

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createSwapChain();

	void createImageViews();

	void createDefaultGraphicsPipeline();

	void createCircleGraphicsPipeline();

	void createGraphicsPipeline(const std::vector<char>& vertexShaderBuffer, const std::vector<char>& fragmentShaderCode, const VkVertexInputBindingDescription& bindingDescription,
		const VkVertexInputAttributeDescription* attributeDescriptions,
		size_t attributeDescriptionsSize,
		VkPipelineLayout& pipelineLayout,
		VkPipeline& graphicsPipeline);

	void createRenderPass();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	void createFramebuffers();

	void createCommandPool();

	void createCommandBuffers();

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	void drawElements(VkCommandBuffer commandBuffer, ElementType type);

	void createSyncObjects();

	void cleanupSwapChain();

	void recreateSwapChain();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createDescriptorSetLayout();

	void createUniformBuffers();

	void createStorageBuffers(GraphicElement* element);

	void updateUniformBuffer(uint32_t currentImage);

	void updateStorageBuffer(GraphicElement* element, uint32_t currentImage);

	void createDescriptorPool();

	void createDescriptorPool(GraphicElement* element);

	void createDescriptorSets(GraphicElement* element);

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	static VkVertexInputBindingDescription getVertexBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static VkVertexInputBindingDescription getCircleVertexBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(CircleVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 1> getVertexAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		return attributeDescriptions;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getCircleVertexAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(CircleVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(CircleVertex, radius);

		return attributeDescriptions;
	}
};

