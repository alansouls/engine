#pragma once

#include "core/Messenger.h"
#include "engine/graphics/drivers/VulkanDriver.h"
#include "engine/graphics/renderers/scene/SceneImage.h"
#include "engine/graphics/renderers/scene/SceneRenderer.h"

#include <memory>

typedef MappedBuffer Camera;

class RendererItem;
enum RendererItemType : uint32_t;
class EditorSceneRenderer : public SSGE::SceneRenderer
{
  public:
    explicit EditorSceneRenderer(VulkanDriver *driver, SSGE::Messenger *messenger, uint32_t width, uint32_t height);
    ~EditorSceneRenderer();

    auto render(uint32_t currentImage) -> SceneImage *;
    [[nodiscard]] auto getImage(uint32_t currentImage) const -> SceneImage *;

    auto resize(uint32_t width, uint32_t height) -> void;

    [[nodiscard]] auto getWidth() const -> uint32_t;
    [[nodiscard]] auto getHeight() const -> uint32_t;

  private:
    VulkanDriver *m_driver;
    VkRenderPass m_renderPass;
    std::array<std::unique_ptr<SceneImage>, MAX_FRAMES_IN_FLIGHT> m_images;
    std::array<VkFramebuffer, MAX_FRAMES_IN_FLIGHT> m_framebuffers;

    auto init(uint32_t width, uint32_t height) -> void;

    friend class RendererItem;

    uint32_t m_resizeWidth[MAX_FRAMES_IN_FLIGHT];
    uint32_t m_resizeHeight[MAX_FRAMES_IN_FLIGHT];

    auto commitResize(uint32_t currentImage) -> void;
    auto cleanupGraphicsResources() -> void;
};
