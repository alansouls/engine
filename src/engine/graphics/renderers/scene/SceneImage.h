#pragma once
#include "engine/graphics/drivers/VulkanDriver.h"
#include "imgui.h"

#include <vulkan/vulkan_core.h>

class SceneImage
{
  public:
    SceneImage(uint32_t width, uint32_t height, VulkanDriver *driver);
    ~SceneImage();

    auto resize(uint32_t width, uint32_t height) -> void;

    [[nodiscard]] auto getUITexture() -> ImTextureID;

    [[nodiscard]] auto getWidth() const -> uint32_t;

    [[nodiscard]] auto getHeight() const -> uint32_t;

    [[nodiscard]] auto getImageView() const -> VkImageView;

    [[nodiscard]] auto getIsReady() const -> bool;

    auto setAsReady() -> void;

    static auto getFromSwapChainImage(VulkanDriver *driver, uint32_t imageIndex) -> std::unique_ptr<SceneImage>;

  private:
    bool m_isReady;
    VkSampler m_sampler;
    VkImageView m_imageView;
    VkImage m_image;
    VkDeviceMemory m_memory;
    VkDescriptorSet m_descriptorSet;
    uint32_t m_width;
    uint32_t m_height;

    VulkanDriver *m_driver;

    SceneImage(const glm::vec2 &extent, VkImageView imageView);
    auto init() -> void;
    auto cleanUpVulkanResources() -> void;
};