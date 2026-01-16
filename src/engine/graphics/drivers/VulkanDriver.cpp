#define GLM_FORCE_RADIANS
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "VulkanDriver.h"
#include "GraphicsOperation.h"
#include "imgui.h"
#include <algorithm>
#include <backends/imgui_impl_vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ranges>
#include <set>

VulkanDriver::VulkanDriver(const std::vector<const char *> &requiredExtensions,
                           const std::vector<const char *> &validationLayers,
                           const std::vector<const char *> &deviceExtensions, GLFWwindow *window,
                           const GraphicsDriverOptions &options)
    : GraphicsDriver(window, options), m_uiDescriptorPool(VK_NULL_HANDLE), m_validationLayers(validationLayers),
      m_deviceExtensions(deviceExtensions), m_requiredExtensions(requiredExtensions), m_instance(), m_debugMessenger(),
      m_surface(), m_logicalDevice(), m_graphicsQueue(), m_presentQueue(), m_swapChain(),
      m_swapChainImageFormat(VK_FORMAT_UNDEFINED), m_swapChainExtent({0, 0}), m_renderPass(), m_defaultPipelineLayout(),
      m_circlePipelineLayout(), m_defaultGraphicsPipeline(), m_circleGraphicsPipeline(), m_commandPool(),
      m_extentFactorWidth(1.0f), m_extentFactorHeight(1.0f)
{
    m_primitives[ElementType::Quad] = {};
    m_primitives[ElementType::Circle] = {};
}

void VulkanDriver::init()
{
    std::cout << "Initializing Vulkan..." << std::endl;
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
    createUIDescriptorPool();
    std::cout << "Finished!" << std::endl;
}

void VulkanDriver::cleanup()
{
    std::cout << "Cleaning up Vulkan resources..." << std::endl;
    cleanupSwapChain();

    vkDestroyDescriptorPool(m_logicalDevice, m_uiDescriptorPool, nullptr);

    for (auto &pair : m_primitives)
    {
        auto &data = pair.second;

        vkDestroyBuffer(m_logicalDevice, data.indexBuffer, nullptr);
        vkFreeMemory(m_logicalDevice, data.indexBufferMemory, nullptr);

        vkDestroyBuffer(m_logicalDevice, data.vertexBuffer, nullptr);
        vkFreeMemory(m_logicalDevice, data.vertexBufferMemory, nullptr);
    }

    vkDestroyPipeline(m_logicalDevice, m_defaultGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_logicalDevice, m_defaultPipelineLayout, nullptr);

    vkDestroyPipeline(m_logicalDevice, m_circleGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_logicalDevice, m_circlePipelineLayout, nullptr);

    vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_logicalDevice, m_inFlightFences[i], nullptr);
    }

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
    }

    vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);

    vkDestroyDevice(m_logicalDevice, nullptr);

    if (m_options.debugModeOn)
    {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    ImGui::DestroyContext();

    vkDestroyInstance(m_instance, nullptr);

    // for (auto &elements : m_elementsByType | std::views::values)
    // {
    //     for (const auto &element : elements)
    //     {
    //         delete element;
    //     }
    // }

    std::cout << "Vulkan resources cleaned up!" << std::endl;
}

auto VulkanDriver::initForUI() const -> void
{
    ImGui_ImplVulkan_InitInfo init_info = {};
    // init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion,
    // otherwise will default to header version.
    init_info.Instance = m_instance;
    init_info.PhysicalDevice = m_physicalDevice;
    init_info.Device = m_logicalDevice;
    init_info.QueueFamily = 0;
    init_info.Queue = m_graphicsQueue;
    init_info.DescriptorPool = m_uiDescriptorPool;
    init_info.RenderPass = m_renderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
    init_info.ImageCount = MAX_FRAMES_IN_FLIGHT;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    ImGui_ImplVulkan_Init(&init_info);
}

auto VulkanDriver::cleanupForUI() -> void
{
    ImGui_ImplVulkan_Shutdown();
}

auto VulkanDriver::beginUIFrame() -> void
{
    ImGui_ImplVulkan_NewFrame();
}

auto VulkanDriver::waitForFence(VkFence fence) const -> void
{
    vkWaitForFences(m_logicalDevice, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
}

auto VulkanDriver::resetFence(VkFence fence) const -> void
{
    vkResetFences(m_logicalDevice, 1, &fence);
}

auto VulkanDriver::getFrameFence(uint32_t currentImage) -> VkFence
{
    return m_inFlightFences[currentImage];
}

void VulkanDriver::drawFrame(uint32_t currentFrame, ImDrawData *drawData)
{
    auto fence = m_inFlightFences[currentFrame];
    auto imageAvailableSemaphore = m_imageAvailableSemaphores[currentFrame];

    uint32_t imageIndex;

    VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, std::numeric_limits<uint64_t>::max(),
                                            imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    resetFence(fence);

    auto renderFinishedSemaphore = m_renderFinishedSemaphores[imageIndex];

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    auto commandBuffer = beginCommandWrite(currentFrame);

    recordCommandBuffer(commandBuffer, imageIndex, drawData);

    submitCommandBuffer(commandBuffer, {imageAvailableSemaphore}, {renderFinishedSemaphore}, fence);

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        m_framebufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

auto VulkanDriver::submitCommandBuffer(VkCommandBuffer commandBuffer, const std::vector<VkSemaphore> &waitSemaphores,
                                       const std::vector<VkSemaphore> &signalSemaphores, VkFence fence) const -> void
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

auto VulkanDriver::beginCommandWrite(uint32_t currentFrame) const -> VkCommandBuffer
{
    VkCommandBuffer commandBuffer = m_commandBuffers[currentFrame];

    vkResetCommandBuffer(commandBuffer, 0);

    return commandBuffer;
}

void VulkanDriver::waitIdle()
{
    vkDeviceWaitIdle(m_logicalDevice);
}

void VulkanDriver::createInstance()
{
    if (m_options.debugModeOn && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Some of the specified validation layers are not supported!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    auto extensions = getRequiredExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_options.debugModeOn)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

#ifdef MACOSX
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create vulkan instance");
    }
}

bool VulkanDriver::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : m_validationLayers)
    {
        bool found = false;
        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }

    return true;
}

std::vector<const char *> VulkanDriver::getRequiredExtensions()
{
    std::vector<const char *> extensions = m_requiredExtensions;

    if (m_options.debugModeOn)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanDriver::setupDebugMessenger()
{
    if (!m_options.debugModeOn)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VulkanDriver::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanDriver::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (const auto &device : devices)
    {
        if (isDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool VulkanDriver::isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    auto indices = findQueueFamilies(device);

    if (!indices.isComplete())
        return false;

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    if (!extensionsSupported)
        return false;

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

    return swapChainAdequate;
}

bool VulkanDriver::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanDriver::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;

        i++;
    }

    return indices;
}

void VulkanDriver::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    if (m_options.debugModeOn)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
}

void VulkanDriver::createSurface()
{
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

SwapChainSupportDetails VulkanDriver::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanDriver::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanDriver::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanDriver::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    m_extentFactorWidth = static_cast<float>(width) / capabilities.currentExtent.width;
    m_extentFactorHeight = static_cast<float>(height) / capabilities.currentExtent.height;

    return capabilities.currentExtent;
}

void VulkanDriver::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    m_swapChainImageFormat = surfaceFormat.format;
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    m_swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = m_swapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_logicalDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, m_swapChainImages.data());
}

void VulkanDriver::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

auto VulkanDriver::createDefaultGraphicsPipeline(VkDescriptorSetLayout descriptorSetLayout) -> void
{
    auto bindingDescription = getVertexBindingDescription();
    auto attributeDescriptions = getVertexAttributeDescriptions();

    createGraphicsPipeline(m_options.defaultVertexShader, m_options.defaultVertexShaderSize,
                           m_options.defaultFragmentShader, m_options.defaultFragmentShaderSize, bindingDescription,
                           attributeDescriptions.data(), attributeDescriptions.size(), descriptorSetLayout,
                           m_defaultPipelineLayout, m_defaultGraphicsPipeline);

    m_primitives[ElementType::Quad].graphicsPipeline = m_defaultGraphicsPipeline;
}

auto VulkanDriver::createCircleGraphicsPipeline(VkDescriptorSetLayout descriptorSetLayout) -> void
{
    auto bindingDescription = getCircleVertexBindingDescription();
    auto attributeDescriptions = getCircleVertexAttributeDescriptions();

    createGraphicsPipeline(m_options.circleVertexShader, m_options.circleVertexShaderSize,
                           m_options.circleFragmentShader, m_options.circleFragmentShaderSize, bindingDescription,
                           attributeDescriptions.data(), attributeDescriptions.size(), descriptorSetLayout,
                           m_circlePipelineLayout, m_circleGraphicsPipeline);

    m_primitives[ElementType::Circle].graphicsPipeline = m_circleGraphicsPipeline;
}

auto VulkanDriver::getSwapChainImageView(uint32_t imageIndex) const -> VkImageView
{
    return m_swapChainImageViews[imageIndex];
}

void VulkanDriver::createGraphicsPipeline(const uint8_t *vertexShaderBuffer, size_t vertexShaderBufferSize,
                                          const uint8_t *fragmentShaderCode, size_t fragShaderBufferSize,
                                          const VkVertexInputBindingDescription &bindingDescription,
                                          const VkVertexInputAttributeDescription *attributeDescriptions,
                                          size_t attributeDescriptionsSize, VkDescriptorSetLayout descriptorSetLayout,
                                          VkPipelineLayout &pipelineLayout, VkPipeline &graphicsPipeline)
{
    VkShaderModule vertShaderModule = createShaderModule(vertexShaderBuffer, vertexShaderBufferSize);
    VkShaderModule fragShaderModule = createShaderModule(fragmentShaderCode, fragShaderBufferSize);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptionsSize);
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    // Draw triangle from 3 vertices
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // default view port and scissor using window size
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapChainExtent.width;
    viewport.height = (float)m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
}

void VulkanDriver::createRenderPass()
{
    m_renderPass = createRenderPass(VK_SAMPLE_COUNT_1_BIT, m_swapChainImageFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

auto VulkanDriver::createRenderPass(VkSampleCountFlagBits samples, VkFormat format, VkImageLayout layout) const
    -> VkRenderPass
{
    VkRenderPass renderPass;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = samples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = layout;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }

    return renderPass;
}

auto VulkanDriver::destroyRenderPass(VkRenderPass renderPass) const -> void
{
    vkDestroyRenderPass(m_logicalDevice, renderPass, nullptr);
}

VkShaderModule VulkanDriver::createShaderModule(const uint8_t *code, size_t codeSize)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void VulkanDriver::createFramebuffers()
{
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        m_swapChainFramebuffers[i] = createFrameBuffer(
            m_renderPass,
            m_swapChainImageViews[i],
            m_swapChainExtent.width,
            m_swapChainExtent.height
        );
    }
}

void VulkanDriver::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void VulkanDriver::createCommandBuffers()
{
    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanDriver::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, ImDrawData *uiData) const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 1.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y =  0.0f;
    viewport.width = static_cast<float>(m_swapChainExtent.width);
    viewport.height = static_cast<float>(m_swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_RenderDrawData(uiData, commandBuffer);

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

auto VulkanDriver::beginRenderPass(VkRenderPass sceneRenderPass, VkCommandBuffer commandBuffer,
                                   VkFramebuffer sceneFrameBuffer, VkExtent2D sceneExtent) -> RenderInfo
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = sceneRenderPass;
    renderPassInfo.framebuffer = sceneFrameBuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = sceneExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 1.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(sceneExtent.width);
    viewport.height = static_cast<float>(sceneExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = sceneExtent;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return {.viewport = viewport, .scissor = scissor};
}

auto VulkanDriver::prepareDraw(VkCommandBuffer commandBuffer, ElementType type, const RenderInfo &renderInfo) const
    -> void
{
    VkPipeline pipeline = type == ElementType::Quad ? m_defaultGraphicsPipeline : m_circleGraphicsPipeline;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdSetViewport(commandBuffer, 0, 1, &renderInfo.viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &renderInfo.scissor);
}

auto VulkanDriver::drawElementInstances(VkCommandBuffer commandBuffer, GraphicElement *element, uint32_t currentFrame)
    -> void
{
    VkPipelineLayout pipelineLayout =
        (element->type == ElementType::Quad ? m_defaultPipelineLayout : m_circlePipelineLayout);

    auto &data = m_primitives[element->type];

    VkBuffer vertexBuffers[] = {data.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, data.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &element->descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(data.indicesSize), element->instanceData.size(), 0, 0, 0);
}

auto VulkanDriver::endRenderPassAndCommandBuffer(VkCommandBuffer commandBuffer) -> void
{
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void VulkanDriver::createSyncObjects()
{
    size_t imageCount = m_swapChainImages.size();
    m_renderFinishedSemaphores.resize(imageCount);
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }

    for (size_t i = 0; i < imageCount; ++i)
    {
        if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}

auto VulkanDriver::createFence(VkFenceCreateFlags createFlags) const -> VkFence
{
    VkFence fence = VK_NULL_HANDLE;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }

    return fence;
}

auto VulkanDriver::destroyFence(VkFence fence) const -> void
{
    vkDestroyFence(m_logicalDevice, fence, nullptr);
}

void VulkanDriver::cleanupSwapChain()
{
    for (auto framebuffer : m_swapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
    }

    for (auto imageView : m_swapChainImageViews)
    {
        vkDestroyImageView(m_logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
}

void VulkanDriver::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_logicalDevice);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();

    std::set<uint32_t> frames;
    for (uint32_t frame = 0; frame < MAX_FRAMES_IN_FLIGHT; ++frame)
    {
        frames.insert(frame);
    }
}

auto VulkanDriver::updateVertexBuffer(const GraphicElement *element) -> void
{
    auto &data = m_primitives[element->type];

    if (data.vertexBuffer != VK_NULL_HANDLE)
    {
        return;
    }

    VkDeviceSize bufferSize = 0;

    switch (element->type)
    {
    case ElementType::Quad:
        bufferSize = sizeof(Vertex) * 4;
        break;
    case ElementType::Circle:
        bufferSize = sizeof(CircleVertex) * 4;
        break;
    default:
        break;
    }

    if (bufferSize == 0)
    {
        throw std::runtime_error("Element cannot have 0 vertices");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                 stagingBufferMemory);

    void *vertexBufferData;

    void *vertexData = nullptr;

    switch (element->type)
    {
    case ElementType::Quad: {
        glm::vec2 normalizedTopLeft = {0.0f, 0.0f};
        float normalizedWidth = 1.0f;
        float normalizedHeight = 1.0f;
        vertexData =
            new Vertex[4]{{normalizedTopLeft},
                          {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y)},
                          {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y + normalizedHeight)},
                          {glm::vec2(normalizedTopLeft.x, normalizedTopLeft.y + normalizedHeight)}};
    }
    break;
    case ElementType::Circle: {
        glm::vec2 normalizedTopLeft = {-0.5f, -0.5f};
        float normalizedWidth = 1.0f;
        float normalizedHeight = 1.0f;
        float radius = 0.5f;
        vertexData = new CircleVertex[4]{
            {normalizedTopLeft, radius},
            {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y), radius},
            {glm::vec2(normalizedTopLeft.x + normalizedWidth, normalizedTopLeft.y + normalizedHeight), radius},
            {glm::vec2(normalizedTopLeft.x, normalizedTopLeft.y + normalizedHeight), radius}};
    }
    break;
    default:
        throw std::runtime_error("Invalid");
    }

    vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &vertexBufferData);
    memcpy(vertexBufferData, vertexData, (size_t)bufferSize);
    vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data.vertexBuffer, data.vertexBufferMemory);

    copyBuffer(stagingBuffer, data.vertexBuffer, bufferSize);

    vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);

    switch (element->type)
    {
    case ElementType::Quad:
        delete[] static_cast<Vertex *>(vertexData);
        break;
    case ElementType::Circle:
        delete[] static_cast<CircleVertex *>(vertexData);
        break;
    default:
        break;
    }
}

auto VulkanDriver::updateIndexBuffer(const GraphicElement *element) -> void
{
    auto &data = m_primitives[element->type];

    if (data.indexBuffer != VK_NULL_HANDLE)
    {
        return;
    }

    VkDeviceSize bufferSize = 0;

    void *indicesData = nullptr;

    switch (element->type)
    {
    case ElementType::Quad:
    case ElementType::Circle:
        bufferSize = sizeof(uint16_t) * 6;
        data.indicesSize = 6;
        indicesData = new uint16_t[6]{0, 1, 2, 2, 3, 0};
        break;
    default:
        break;
    }

    if (bufferSize == 0)
    {
        throw std::runtime_error("Element cannot have 0 indices");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                 stagingBufferMemory);

    void *indexBufferData;
    vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &indexBufferData);
    memcpy(indexBufferData, indicesData, (size_t)bufferSize);
    vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, data.indexBuffer, data.indexBufferMemory);

    copyBuffer(stagingBuffer, data.indexBuffer, bufferSize);

    vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);

    delete[] static_cast<uint16_t *>(indicesData);
}

uint32_t VulkanDriver::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanDriver::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_logicalDevice, buffer, bufferMemory, 0);
}

void VulkanDriver::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
}

auto VulkanDriver::create2DImage(const uint32_t width, const uint32_t height) const -> VkImage
{
    VkImage image;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage =
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    return image;
}

auto VulkanDriver::createAndBindImageMemory(VkImage image) const -> VkDeviceMemory
{
    VkDeviceMemory memory;

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_logicalDevice, image, memory, 0);

    return memory;
}

auto VulkanDriver::createImageView(VkImage image, VkFormat format) const -> VkImageView
{
    VkImageView imageView;

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

auto VulkanDriver::createTextureSampler(VkImage image, VkFormat format) const -> VkSampler
{
    VkSampler sampler;

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }

    return sampler;
}

auto VulkanDriver::createFrameBuffer(VkRenderPass renderPass, VkImageView imageView, uint32_t width,
                                     uint32_t height) const -> VkFramebuffer
{
    VkFramebuffer framebuffer;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &imageView;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }

    return framebuffer;
}

auto VulkanDriver::destroyFrameBuffer(VkFramebuffer frameBuffer) const -> void
{
    vkDestroyFramebuffer(m_logicalDevice, frameBuffer, nullptr);
}

auto VulkanDriver::destroyImage(VkImage image) const -> void
{
    vkDestroyImage(m_logicalDevice, image, nullptr);
}

auto VulkanDriver::destroyImageView(VkImageView imageView) const -> void
{
    vkDestroyImageView(m_logicalDevice, imageView, nullptr);
}

auto VulkanDriver::destroyTextureSampler(VkSampler sampler) const -> void
{
    vkDestroySampler(m_logicalDevice, sampler, nullptr);
}

auto VulkanDriver::freeMemory(VkDeviceMemory memory) const -> void
{
    vkFreeMemory(m_logicalDevice, memory, nullptr);
}

auto VulkanDriver::createDescriptorSetLayout(const std::vector<DescriptorSetCreateData> &data) const
    -> VkDescriptorSetLayout
{
    std::vector<VkDescriptorSetLayoutBinding> bindings(data.size());

    for (int i = 0; i < data.size(); i++)
    {
        const auto &[descriptorType, stageFlags, descriptorCount] = data[i];

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = i;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = descriptorCount;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr; // Optional

        bindings[i] = layoutBinding;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    VkDescriptorSetLayout descriptorSetLayout;

    if (vkCreateDescriptorSetLayout(m_logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return descriptorSetLayout;
}

auto VulkanDriver::destroyDescriptorSetLayout(const VkDescriptorSetLayout layout) const -> void
{
    vkDestroyDescriptorSetLayout(m_logicalDevice, layout, nullptr);
}

auto VulkanDriver::createMappedBuffer(const size_t size, VkBufferUsageFlags usage) -> MappedBuffer
{
    VkDeviceSize bufferSize = size;

    MappedBuffer mappedBuffer{.size = size};

    createBuffer(bufferSize, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 mappedBuffer.buffer, mappedBuffer.bufferMemory);

    vkMapMemory(m_logicalDevice, mappedBuffer.bufferMemory, 0, bufferSize, 0, &mappedBuffer.bufferMapped);

    return mappedBuffer;
}

auto VulkanDriver::freeMappedBuffer(const MappedBuffer &mappedBuffer) const -> void
{
    vkDestroyBuffer(m_logicalDevice, mappedBuffer.buffer, nullptr);
    vkFreeMemory(m_logicalDevice, mappedBuffer.bufferMemory, nullptr);
}

void VulkanDriver::createUIDescriptorPool()
{
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 2},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
    for (auto &[type, descriptorCount] : pool_sizes)
        pool_info.maxSets += descriptorCount;
    pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(m_logicalDevice, &pool_info, nullptr, &m_uiDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create ui descriptor pool!");
    }
}

auto VulkanDriver::createDescriptorPool(const std::vector<VkDescriptorType> &types, const uint32_t count) const
    -> VkDescriptorPool
{
    VkDescriptorPool descriptorPool;

    std::vector<VkDescriptorPoolSize> poolSizes(types.size());

    for (int i = 0; i < types.size(); i++)
    {
        poolSizes[i].type = types[i];
        poolSizes[i].descriptorCount = count;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = count;

    if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    return descriptorPool;
}

auto VulkanDriver::destroyDescriptorPool(VkDescriptorPool descriptorPool) const -> void
{
    vkDestroyDescriptorPool(m_logicalDevice, descriptorPool, nullptr);
}

auto VulkanDriver::writeDescriptorSet(VkDescriptorSet descriptorSet, const MappedBuffer &mappedBuffer,
                                      VkDescriptorType descriptorType, const uint32_t binding) const -> void
{
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = mappedBuffer.buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = mappedBuffer.size;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.descriptorCount = 1;

    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;       // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(m_logicalDevice, 1, &descriptorWrite, 0, nullptr);
}

auto VulkanDriver::createDescriptorSets(uint32_t count, VkDescriptorPool descriptorPool,
                                        const std::vector<VkDescriptorSetLayout> &layouts) const
    -> std::vector<VkDescriptorSet>
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> descriptorSets(count);

    if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    return descriptorSets;
}
