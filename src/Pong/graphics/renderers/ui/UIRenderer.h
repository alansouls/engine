#pragma once

#include "../scene/EditorSceneRenderer.h"
#include "core/Messenger.h"
#include "engine/graphics/EngineWindow.h"
#include "engine/graphics/drivers/VulkanDriver.h"
#include "game/EngineGame.h"
#include "views/UIView.h"

class EngineWindow;
class UIRenderer
{
  public:
    UIRenderer(SSGE::Editor::EngineGame *game, EngineWindow *window, VulkanDriver *driver);
    ~UIRenderer();
    auto init(EditorSceneRenderer *sceneRenderer, EditorSceneRenderer *gameSceneRenderer) -> void;
    auto renderMenu() const -> void;
    [[nodiscard]] auto renderUI(uint32_t currentImage) const -> ImDrawData *;

  private:
    std::unique_ptr<SSGE::Messenger> m_messenger;
    SSGE::Editor::EngineGame *m_game;
    EngineWindow *m_window;
    VulkanDriver *m_driver;
    std::vector<std::unique_ptr<UIView>> m_views;

    mutable bool m_showNewProjectForm = false;
    mutable char m_newProjectName[256] = {};
    mutable std::string m_newProjectFolderPath;

    auto renderStartupPopup() const -> void;
};
