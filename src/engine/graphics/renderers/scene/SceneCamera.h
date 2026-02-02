#pragma once
#include "engine/graphics/drivers/VulkanDriver.h"
#include "graphics/utils/Transform.h"

namespace SSGE
{
class SceneCamera
{
  public:
    explicit SceneCamera(VulkanDriver *driver);
    SceneCamera(const SceneCamera &camera) = delete;
    ~SceneCamera();

    [[nodiscard]] auto getBuffer(uint32_t frameIndex) const -> const MappedBuffer &;

    auto update(uint32_t width, uint32_t height, uint32_t currentFrame) -> void;

  private:
    Transform m_transform{};
    VulkanDriver *m_driver;
    std::array<TypedMappedBuffer<UniformBufferObject>, MAX_FRAMES_IN_FLIGHT> m_buffers{};
    std::array<Resolution, MAX_FRAMES_IN_FLIGHT> m_resolutions{};

    auto initGraphicsResources() -> void;
    auto cleanupGraphicResources() -> void;
};
} // namespace SSGE