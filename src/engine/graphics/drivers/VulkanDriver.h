#pragma once

#include "../utils/UniformBufferObject.h"
#include "GraphicsDriver.h"
#include <cstddef>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

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
    size_t indicesSize;
};

template <typename utype> struct TypedMappedBuffer
{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    union {
        utype *typedBufferMapped;
        void *voidBufferMapped;
    };
    size_t size;
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

struct GraphicsPipelineInfo
{
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};

typedef VkExtent2D Resolution;
class VulkanDriver : public GraphicsDriver
{
  public:
    VulkanDriver(const std::vector<const char *> &requiredExtensions, const std::vector<const char *> &validationLayers,
                 const std::vector<const char *> &deviceExtensions, GLFWwindow *window,
                 const GraphicsDriverOptions &options);
    ~VulkanDriver() override;

    auto init() -> void override;

    auto cleanup() -> void override;

    auto initForUI(uint32_t imagesToRender) -> void;

    auto cleanupForUI() const -> void;

    static auto beginUIFrame() -> void;
    auto waitForFence(VkFence fence) const -> void;
    auto resetFence(VkFence fence) const -> void;
    auto getFrameFence(uint32_t currentImage) -> VkFence;

    auto drawFrame(uint32_t currentFrame, ImDrawData *drawData) -> void override;

    auto submitCommandBuffer(VkCommandBuffer commandBuffer, const std::vector<VkSemaphore> &waitSemaphores,
                             const std::vector<VkSemaphore> &signalSemaphores, VkFence fence) const -> void;

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
    auto destroyFrameBuffer(VkFramebuffer frameBuffer) const -> void;
    [[nodiscard]] auto createRenderPass(VkSampleCountFlagBits samples, VkFormat format, VkImageLayout layout) const
        -> VkRenderPass;
    auto destroyRenderPass(VkRenderPass renderPass) const -> void;
    auto destroyImage(VkImage image) const -> void;
    auto destroyImageView(VkImageView imageView) const -> void;
    auto destroyTextureSampler(VkSampler sampler) const -> void;
    auto freeMemory(VkDeviceMemory memory) const -> void;
    auto createMappedBuffer(size_t size, VkBufferUsageFlags usage) -> MappedBuffer;
    auto freeMappedBuffer(const MappedBuffer &mappedBuffer) const -> void;

    auto writeDescriptorSet(VkDescriptorSet descriptorSet, const MappedBuffer &mappedBuffer,
                            VkDescriptorType descriptorType, uint32_t binding) const -> void;
    auto createDescriptorSets(uint32_t count, VkDescriptorPool descriptorPool,
                              const std::vector<VkDescriptorSetLayout> &layouts) const -> std::vector<VkDescriptorSet>;
    auto destroyPipelineInfo(const GraphicsPipelineInfo &info) const -> void;

    [[nodiscard]] auto createDescriptorPool(const std::vector<VkDescriptorType> &types, uint32_t count) const
        -> VkDescriptorPool;
    auto destroyDescriptorPool(VkDescriptorPool descriptorPool) const -> void;

    [[nodiscard]] auto createDescriptorSetLayout(const std::vector<DescriptorSetCreateData> &data) const
        -> VkDescriptorSetLayout;
    auto destroyDescriptorSetLayout(VkDescriptorSetLayout layout) const -> void;

    auto updateVertexBuffer(const GraphicElement *element, PrimitiveData &data) -> void;
    auto updateIndexBuffer(const GraphicElement *element, PrimitiveData &data) -> void;

    static auto beginRenderPass(VkRenderPass sceneRenderPass, VkCommandBuffer commandBuffer,
                                VkFramebuffer sceneFrameBuffer, VkExtent2D sceneExtent) -> RenderInfo;
    static auto prepareDraw(VkCommandBuffer commandBuffer, const RenderInfo &renderInfo, VkPipeline pipeline) -> void;
    static auto drawElementInstances(VkCommandBuffer commandBuffer, GraphicElement *element, uint32_t currentFrame,
                                     VkPipelineLayout pipelineLayout, const PrimitiveData &data) -> void;
    static auto endRenderPassAndCommandBuffer(VkCommandBuffer commandBuffer) -> void;

    auto createDefaultGraphicsPipeline(VkDescriptorSetLayout descriptorSetLayout) const -> GraphicsPipelineInfo;

    auto createCircleGraphicsPipeline(VkDescriptorSetLayout descriptorSetLayout) const -> GraphicsPipelineInfo;

    [[nodiscard]] auto getSwapChainImageView(uint32_t imageIndex) const -> VkImageView;

    template <typename utype> auto createMappedBuffer(VkBufferUsageFlags usage) -> TypedMappedBuffer<utype>;

    template <typename utype> auto freeMappedBuffer(const TypedMappedBuffer<utype> &buffer) -> void;

  private:
    VkDescriptorPool m_uiDescriptorPool;

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

    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    VkCommandPool m_commandPool;

    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

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

    auto createGraphicsPipeline(const uint8_t *vertexShaderBuffer, size_t vertexShaderBufferSize,
                                const uint8_t *fragmentShaderCode, size_t fragShaderBufferSize,
                                const VkVertexInputBindingDescription &bindingDescription,
                                const VkVertexInputAttributeDescription *attributeDescriptions,
                                size_t attributeDescriptionsSize, VkDescriptorSetLayout descriptorSetLayout,
                                VkPipelineLayout &pipelineLayout, VkPipeline &graphicsPipeline) const -> void;

    void createRenderPass();

    auto createShaderModule(const uint8_t *code, size_t codeSize) const -> VkShaderModule;

    void createFramebuffers();

    void createCommandPool();

    void createCommandBuffers();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, ImDrawData *uiData) const;

    void createSyncObjects();
    [[nodiscard]] auto createFence(VkFenceCreateFlags createFlags) const -> VkFence;
    auto destroyFence(VkFence fence) const -> void;

    void cleanupSwapChain();

    void recreateSwapChain();

    [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    [[nodiscard]] auto createUIDescriptorPool(uint32_t numberOfTextures) const -> VkDescriptorPool;

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

// Template function definitions
template <typename utype> auto VulkanDriver::createMappedBuffer(VkBufferUsageFlags usage) -> TypedMappedBuffer<utype>
{
    auto mappedBuffer = createMappedBuffer(sizeof(utype), usage);

    return TypedMappedBuffer<utype>{
        .buffer = mappedBuffer.buffer,
        .bufferMemory = mappedBuffer.bufferMemory,
        .typedBufferMapped = static_cast<utype *>(mappedBuffer.bufferMapped),
        .size = mappedBuffer.size,
    };
}

template <typename utype> auto VulkanDriver::freeMappedBuffer(const TypedMappedBuffer<utype> &buffer) -> void
{
    freeMappedBuffer(*reinterpret_cast<const MappedBuffer *>(&buffer));
}