#include "SceneCamera.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace SSGE
{

SceneCamera::SceneCamera(VulkanDriver *driver) : m_driver(driver), m_buffer()
{
    init();
}

SceneCamera::~SceneCamera()
{
    cleanupGraphicResources();
}

auto SceneCamera::init() -> void
{
    m_buffer = m_driver->createMappedBuffer<UniformBufferObject>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

auto SceneCamera::cleanupGraphicResources() -> void
{
    m_driver->freeMappedBuffer(m_buffer);
    m_buffer = {}; // TODO: is this necessary?
}

auto SceneCamera::update(uint32_t width, uint32_t height) -> void
{
    if (m_resolution.width == width && m_resolution.height == height)
    {
        return;
    }

    UniformBufferObject ubo{};

    ubo.view = glm::mat4(1.0f);
    ubo.proj = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1000.0f, 1000.0f);

    *m_buffer.typedBufferMapped = ubo;
    m_resolution.width = width;
    m_resolution.height = height;
}
} // namespace SSGE