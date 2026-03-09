#pragma once

#include "engine/graphics/drivers/VulkanDriver.h"
#include "graphics/EngineWindow.h"
#include "scene/RendererItem.h"

#include <memory>
#include <optional>

class VulkanDriver;
namespace SSGE
{
class SceneRenderer;

struct RendererOptions
{
    bool debugModeOn = false;
    std::optional<uint32_t> fpsCap;
};

class Renderer
{
  public:
    Renderer(EngineWindow *window, const RendererOptions &options);
    virtual ~Renderer() = default;

    auto render() -> void;
    auto addItem(RendererItem *item) const -> void;

    [[nodiscard]] auto getWidth() const -> uint32_t;

    [[nodiscard]] auto getHeight() const -> uint32_t;

    [[nodiscard]] virtual auto getSceneWidth() const -> uint32_t = 0;
    [[nodiscard]] virtual auto getSceneHeight() const -> uint32_t = 0;

  protected:
    std::unique_ptr<VulkanDriver> m_driver;
    EngineWindow *m_window;
    RendererOptions m_options;
    std::vector<SceneRenderer *> m_sceneRenderers;

    virtual auto preRender(uint32_t currentFrame) -> void;
    virtual auto drawFrame(uint32_t frameIndex) -> void = 0;
    virtual auto postRender(uint32_t currentFrame) -> void;

  private:
    uint32_t m_currentImage;

    auto initWindow(EngineWindow *mainWindow) const -> void;
    auto initGraphicsDriver() -> void;

    static std::vector<const char *> getVulkanRequiredExtensions();

    auto framebufferResizeCallback(int width, int height) const -> void;
};

} // namespace SSGE
