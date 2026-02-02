#pragma once

#include "../scene/EditorSceneRenderer.h"
#include "engine/graphics/EngineWindow.h"
#include "engine/graphics/drivers/VulkanDriver.h"
#include "views/UIView.h"

class EngineWindow;
class UIRenderer
{
  public:
    UIRenderer(EngineWindow *window, VulkanDriver *driver);
    ~UIRenderer();
    auto init(EditorSceneRenderer *sceneRenderer, EditorSceneRenderer *gameSceneRenderer) -> void;
    auto renderMenu() const -> void;
    [[nodiscard]] auto renderUI(uint32_t currentImage) const -> ImDrawData *;

  private:
    EngineWindow *m_window;
    VulkanDriver *m_driver;
    std::unique_ptr<SSGE::UIMessenger> m_messenger;
    std::vector<std::unique_ptr<UIView>> m_views;
};