#include "../../drivers/GraphicsOperation.h"
#include "EditorSceneRenderer.h"
#include "RendererItem.h"
#include "engine/graphics/utils/UniformBufferObject.h"
#include <ranges>

EditorSceneRenderer::EditorSceneRenderer(VulkanDriver *driver, uint32_t width, uint32_t height)
    : m_driver(driver), m_renderPass(VK_NULL_HANDLE), m_framebuffers({VK_NULL_HANDLE}), m_cameras({}),
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
        for (auto &elements : m_elementsByType | std::views::values)
        {
            for (const auto element : elements)
            {
                m_driver->freeMappedBuffer(element->storageBuffers[i]);
            }
        }

        m_driver->freeMappedBuffer(m_cameras[i]);
        m_driver->destroyFrameBuffer(m_framebuffers[i]);
    }

    for (auto &elements : m_elementsByType | std::views::values)
    {
        for (const auto element : elements)
        {
            m_driver->destroyDescriptorPool(element->descriptorPool);
        }
    }

    m_driver->destroyRenderPass(m_renderPass);
    m_driver->destroyDescriptorSetLayout(m_descriptorSetLayout);
}

auto EditorSceneRenderer::render(const uint32_t currentImage) -> std::shared_ptr<SceneImage>
{

    image->setAsReady();

    return image;
}

auto EditorSceneRenderer::getImage(const uint32_t currentImage) -> std::shared_ptr<SceneImage>
{
    if (currentImage >= MAX_FRAMES_IN_FLIGHT)
    {
        throw std::invalid_argument("Invalid index");
    }

    return m_images[currentImage];
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
    const auto &image = m_images[currentImage];

    image->resize(m_resizeWidth[currentImage], m_resizeHeight[currentImage]);

    m_driver->destroyFrameBuffer(m_framebuffers[currentImage]);

    m_framebuffers[currentImage] =
        m_driver->createFrameBuffer(m_renderPass, image->getImageView(), image->getWidth(), image->getHeight());

    m_resizeWidth[currentImage] = -1;
    m_resizeHeight[currentImage] = -1;
}

auto EditorSceneRenderer::addItem(RendererItem *item) -> void
{
    item->addCallback(this, &itemUpdated);
    m_addedSet.insert(item);
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
        m_images[i] = std::make_shared<SceneImage>(width, height, m_driver);
    }

    m_renderPass = m_driver->createRenderPass(VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_SRGB,
                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_descriptorSetLayout =
        m_driver->createDescriptorSetLayout({{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1},
                                             {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1}});

    m_driver->createDefaultGraphicsPipeline(m_descriptorSetLayout);
    m_driver->createCircleGraphicsPipeline(m_descriptorSetLayout);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        m_cameras[i] = m_driver->createMappedBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        m_framebuffers[i] = m_driver->createFrameBuffer(m_renderPass, m_images[i]->getImageView(),
                                                        m_images[i]->getWidth(), m_images[i]->getHeight());
    }
}
