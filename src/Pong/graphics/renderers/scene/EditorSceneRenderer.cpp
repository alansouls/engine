#include "EditorSceneRenderer.h"
#include "engine/graphics/drivers/GraphicsOperation.h"
#include "graphics/renderers/scene/SceneRenderer.h"
#include <vulkan/vulkan_core.h>

EditorSceneRenderer::EditorSceneRenderer(VulkanDriver *driver, uint32_t width, uint32_t height)
    : SceneRenderer(driver), m_driver(driver), m_renderPass(VK_NULL_HANDLE), m_framebuffers({VK_NULL_HANDLE}),
      m_resizeWidth(-1), m_resizeHeight(-1)
{
    init(width, height);
}

EditorSceneRenderer::~EditorSceneRenderer()
{
    cleanupGraphicsResources();
}

auto EditorSceneRenderer::cleanupGraphicsResources() -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_driver->destroyFrameBuffer(m_framebuffers[i]);
    }

    m_driver->destroyRenderPass(m_renderPass);
    m_renderPass = nullptr;
}

auto EditorSceneRenderer::render(const uint32_t currentImage) -> SceneImage *
{
    SceneImage *image = m_images[currentImage].get();

    commitResize(currentImage);

    const Resolution &resolution = image->getResolution();

    if (resolution.width == 0 || resolution.height == 0)
    {
        return image;
    }

    SceneRenderer::render(currentImage, resolution, m_driver->getFrameFence(currentImage), m_framebuffers[currentImage],
                          m_renderPass, {}, {});

    image->setAsReady();

    return image;
}

auto EditorSceneRenderer::getImage(const uint32_t currentImage) const -> SceneImage *
{
    return m_images[currentImage].get();
}

auto EditorSceneRenderer::resize(const uint32_t width, const uint32_t height) -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_resizeWidth[i] = width;
        m_resizeHeight[i] = height;
    }
}

auto EditorSceneRenderer::commitResize(uint32_t currentImage) -> void
{
    if (m_resizeWidth[currentImage] == -1 && m_resizeHeight[currentImage] == -1)
    {
        return;
    }

    const auto &image = m_images[currentImage];

    image->resize(m_resizeWidth[currentImage], m_resizeHeight[currentImage]);

    VkFramebuffer framebuffer = m_framebuffers[currentImage];

    if (framebuffer != VK_NULL_HANDLE)
    {
        m_driver->destroyFrameBuffer(m_framebuffers[currentImage]);
    }

    if (image->getWidth() > 0 && image->getHeight() > 0)
    {
        m_framebuffers[currentImage] =
            m_driver->createFrameBuffer(m_renderPass, image->getImageView(), image->getWidth(), image->getHeight());
    }

    m_resizeWidth[currentImage] = -1;
    m_resizeHeight[currentImage] = -1;
}

auto EditorSceneRenderer::getWidth() const -> uint32_t
{
    // TODO should we improve this?
    return m_images[0]->getWidth();
}

auto EditorSceneRenderer::getHeight() const -> uint32_t
{
    // TODO should we improve this?
    return m_images[0]->getHeight();
}

auto EditorSceneRenderer::init(uint32_t width, uint32_t height) -> void
{
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_images[i] = std::make_unique<SceneImage>(width, height, m_driver);
    }

    m_renderPass = m_driver->createRenderPass(VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_SRGB,
                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    if (width == 0 || height == 0)
    {
        return;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_framebuffers[i] = m_driver->createFrameBuffer(m_renderPass, m_images[i]->getImageView(),
                                                        m_images[i]->getWidth(), m_images[i]->getHeight());
    }
}
