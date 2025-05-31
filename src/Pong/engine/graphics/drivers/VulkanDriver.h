#pragma once

#include "GraphicsDriver.h"
#include <cstddef>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include "../utils/UniformBufferObject.h"

constexpr int MAX_FRAMES_IN_FLIGHT = 2;
constexpr int MAX_INSTANCES = 10000;

struct ImDrawData;

struct RenderInfo
{
    VkViewport viewport;
    VkRect2D scissor;
};

struct DescriptorSetCreateData
{
    VkDescriptorType descriptorType;
    VkShaderStageFlags stageFlags;
    uint32_t descriptorCount;
};

struct PrimitiveData
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkPipeline graphicsPipeline;
    size_t indicesSize;
};

struct MappedBuffer
{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *bufferMapped;
    size_t size;
};

struct GraphicElement
{
    GraphicsDriver::ElementType type;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<MappedBuffer> storageBuffers;
    std::vector<InstanceData> instanceData;
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
class VulkanDriver : public GraphicsDriver
{
  public:
    VulkanDriver(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &validationLayers,
                 const std::vector<const char *> &deviceExtensions, GLFWwindow *window,
                 const GraphicsDriverOptions &options);
    ~VulkanDriver() override = default;

    auto init() -> void override;

    auto cleanup() -> void override;

    auto initForUI() const -> void;

    static auto cleanupForUI() -> void;

    static auto beginUIFrame() -> void;

    auto drawFrame(uint32_t currentFrame, ImDrawData *drawData) -> void override;

    auto submitCommandBuffer(VkCommandBuffer commandBuffer, const std::vector<VkSemaphore> &waitSemaphores,
                             const std::vector<VkSemaphore> &signalSemaphores, uint32_t currentFrame) const -> void;

    [[nodiscard]] auto beginCommandWrite(uint32_t currentFrame) const -> VkCommandBuffer;

    auto waitIdle() -> void override;

    [[nodiscard]] auto getWindowSize() const -> glm::vec2 override
    {
        return {m_swapChainExtent.width, m_swapChainExtent.height};
    }

    [[nodiscard]] auto create2DImage(uint32_t width, uint32_t height) const -> VkImage;
    [[nodiscard]] auto createAndBindImageMemory(VkImage image) const -> VkDeviceMemory;
    [[nodiscard]] auto createImageView(VkImage image, VkFormat format) const -> VkImageView;
    auto createTextureSampler(VkImage image, VkFormat format) const -> VkSampler;
    auto createFrameBuffer(VkRenderPass renderPass, VkImageView imageView, uint32_t width, uint32_t height) const
        -> VkFramebuffer;
    [[nodiscard]] auto createRenderPass(VkSampleCountFlagBits samples, VkFormat format, VkImageLayout layout) const
        -> VkRenderPass;
    auto destroyRenderPass(VkRenderPass renderPass) const -> void;
    auto destroyImage(VkImage image) const -> void;
    auto destroyImageView(VkImageView imageView) const -> void;
    auto destroyTextureSampler(VkSampler sampler) const -> void;
    auto freeMemory(VkDeviceMemory memory) const -> void;
    auto createMappedBuffer(size_t size, VkBufferUsageFlags usage) -> MappedBuffer;

    auto writeDescriptorSet(VkDescriptorSet descriptorSet, const MappedBuffer &mappedBuffer,
                            VkDescriptorType descriptorType, uint32_t binding) const -> void;
    auto createDescriptorSets(uint32_t count, VkDescriptorPool descriptorPool,
                              const std::vector<VkDescriptorSetLayout> &layouts) const -> std::vector<VkDescriptorSet>;

    [[nodiscard]] auto createDescriptorPool(const std::vector<VkDescriptorType> &types, uint32_t count) const
        -> VkDescriptorPool;

    [[nodiscard]] auto createDescriptorSetLayout(const std::vector<DescriptorSetCreateData> &data) const
        -> VkDescriptorSetLayout;

    auto updateVertexBuffer(const GraphicElement *element) -> void;
    auto updateIndexBuffer(const GraphicElement *element) -> void;

    static auto beginRenderPass(VkRenderPass sceneRenderPass, VkCommandBuffer commandBuffer,
                                VkFramebuffer sceneFrameBuffer, VkExtent2D sceneExtent) -> RenderInfo;
    auto prepareDraw(VkCommandBuffer commandBuffer, ElementType type, const RenderInfo &renderInfo) const -> void;
    auto drawElementInstances(VkCommandBuffer commandBuffer, GraphicElement *element, uint32_t currentFrame) -> void;
    static auto endRenderPassAndCommandBuffer(VkCommandBuffer commandBuffer) -> void;

    auto createDefaultGraphicsPipeline(VkDescriptorSetLayout descriptorSetLayout) -> void;

    auto createCircleGraphicsPipeline(VkDescriptorSetLayout descriptorSetLayout) -> void;

  private:
    VkDescriptorPool m_uiDescriptorPool;

    std::map<ElementType, PrimitiveData> m_primitives;

    std::vector<const char *> m_validationLayers;
    std::vector<const char *> m_deviceExtensions;
    std::vector<const char *> m_requiredExtensions;
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

    float m_extentFactorWidth;
    float m_extentFactorHeight;

    void createInstance();
    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();
    void setupDebugMessenger();

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void pickPhysicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    void createLogicalDevice();

    void createSurface();

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createSwapChain();

    void createImageViews();

    void createGraphicsPipeline(const uint8_t *vertexShaderBuffer, size_t vertexShaderBufferSize,
                                const uint8_t *fragmentShaderCode, size_t fragShaderBufferSize,
                                const VkVertexInputBindingDescription &bindingDescription,
                                const VkVertexInputAttributeDescription *attributeDescriptions,
                                size_t attributeDescriptionsSize, VkDescriptorSetLayout descriptorSetLayout,
                                VkPipelineLayout &pipelineLayout, VkPipeline &graphicsPipeline);

    void createRenderPass();

    VkShaderModule createShaderModule(const uint8_t *code, size_t codeSize);

    void createFramebuffers();

    void createCommandPool();

    void createCommandBuffers();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, ImDrawData *uiData) const;

    void recordSceneCommandBuffer(VkRenderPass sceneRenderPass, VkCommandBuffer commandBuffer,
                                  VkFramebuffer sceneFrameBuffer, VkExtent2D sceneExtent);

    void drawElements(VkCommandBuffer commandBuffer, ElementType type);

    void createSyncObjects();

    void cleanupSwapChain();

    void recreateSwapChain();

    [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    void createUIDescriptorPool();

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                 const VkAllocationCallbacks *pAllocator,
                                                 VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator)
    {
        auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData)
    {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    static VkVertexInputBindingDescription getVertexBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static VkVertexInputBindingDescription getCircleVertexBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(CircleVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 1> getVertexAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        return attributeDescriptions;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getCircleVertexAttributeDescriptions()
    {
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
