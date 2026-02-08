#include "SceneCamera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace SSGE
{

SceneCamera::SceneCamera(VulkanDriver *driver) : m_driver(driver)
{
    initGraphicsResources();
}

SceneCamera::~SceneCamera()
{
    cleanupGraphicResources();
}

auto SceneCamera::initGraphicsResources() -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_buffers[i] = m_driver->createMappedBuffer<UniformBufferObject>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
}

auto SceneCamera::cleanupGraphicResources() -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_driver->freeMappedBuffer(m_buffers[i]);
    }
    m_buffers = {}; // TODO: is this necessary?
}

auto SceneCamera::getBuffer(uint32_t frameIndex) const -> const MappedBuffer &
{
    return *reinterpret_cast<const MappedBuffer *>(&m_buffers[frameIndex]);
}

auto SceneCamera::update(uint32_t width, uint32_t height, uint32_t currentFrame) -> void
{
    TypedMappedBuffer<UniformBufferObject> &buffer = m_buffers[currentFrame];

    Resolution &resolution = m_resolutions[currentFrame];

    if (resolution.width == width && resolution.height == height)
    {
        return;
    }

    UniformBufferObject ubo{};

    ubo.view = glm::mat4(1.0f);
    ubo.view = glm::translate(ubo.view, m_transform.getPosition());
    ubo.proj = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1000.0f, 1000.0f);

    *buffer.typedBufferMapped = ubo;
    resolution.width = width;
    resolution.height = height;
}
} // namespace SSGE