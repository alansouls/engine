#include "SceneImage.h"

#include "backends/imgui_impl_vulkan.h"
#include "engine/graphics/drivers/VulkanDriver.h"

SceneImage::SceneImage(uint32_t width, uint32_t height, VulkanDriver *driver)
    : m_isReady(false), m_sampler(VK_NULL_HANDLE), m_imageView(VK_NULL_HANDLE), m_image(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE),
      m_descriptorSet(VK_NULL_HANDLE), m_width(width), m_height(height), m_driver(driver)
{
    init();
}

SceneImage::~SceneImage()
{
    cleanUpVulkanResources();
}

auto SceneImage::resize(uint32_t width, uint32_t height) -> void
{
    cleanUpVulkanResources();
    m_width = width;
    m_height = height;
    init();
}

auto SceneImage::getUITexture() -> ImTextureID
{
    if (m_descriptorSet == VK_NULL_HANDLE)
    {
        m_descriptorSet = ImGui_ImplVulkan_AddTexture(m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    return reinterpret_cast<ImTextureID>(m_descriptorSet);
}

auto SceneImage::getWidth() const -> uint32_t
{
    return m_width;
}

auto SceneImage::getHeight() const -> uint32_t
{
    return m_height;
}

auto SceneImage::getImageView() const -> VkImageView
{
    return m_imageView;
}

auto SceneImage::setAsReady() -> void
{
    m_isReady = true;
}

auto SceneImage::getIsReady() const -> bool
{
    return m_isReady;
}

auto SceneImage::init() -> void
{
    m_isReady = false;

    // create and allocate image
    m_image = m_driver->create2DImage(m_width, m_height);
    m_memory = m_driver->createAndBindImageMemory(m_image);

    // create image view, sampler, and layout
    m_imageView = m_driver->createImageView(m_image, VK_FORMAT_B8G8R8A8_SRGB);
    m_sampler = m_driver->createTextureSampler(m_image, VK_FORMAT_B8G8R8A8_SRGB);
}

void SceneImage::cleanUpVulkanResources()
{
    ImGui_ImplVulkan_RemoveTexture(m_descriptorSet);
    m_descriptorSet = VK_NULL_HANDLE;

    m_driver->destroyImageView(m_imageView);
    m_driver->destroyTextureSampler(m_sampler);

    m_driver->freeMemory(m_memory);
    m_driver->destroyImage(m_image);
}