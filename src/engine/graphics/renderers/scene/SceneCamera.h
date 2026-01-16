#pragma once
#include "engine/graphics/drivers/VulkanDriver.h"

namespace SSGE
{
class SceneCamera
{
  public:
    explicit SceneCamera(VulkanDriver *driver);
    ~SceneCamera();

    auto update(uint32_t width, uint32_t height) -> void;
  private:
    VulkanDriver *m_driver;
    TypedMappedBuffer<UniformBufferObject> m_buffer;
    VkExtent2D m_resolution;

    auto init() -> void;
    auto cleanupGraphicResources() -> void;
};
} // namespace SSGE