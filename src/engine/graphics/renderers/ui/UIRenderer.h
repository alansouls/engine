#pragma once

#include "../../drivers/VulkanDriver.h"
#include "engine/graphics/renderers/scene/EditorSceneRenderer.h"
#include "views/UIView.h"

class EngineWindow;
class UIRenderer
{
  public:
    UIRenderer(EngineWindow* window, VulkanDriver *driver);
    ~UIRenderer();
    auto init(EditorSceneRenderer *sceneRenderer) -> void;
    auto renderMenu() const -> void;
    [[nodiscard]] auto renderUI(uint32_t currentImage) const -> ImDrawData *;
    static auto cleanup() -> void;

  private:
    EngineWindow *m_window;
    VulkanDriver *m_driver;

    std::vector<std::unique_ptr<UIView>> m_views;
};