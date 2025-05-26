#pragma once

#include "../../drivers/VulkanDriver.h"

class EngineWindow;
class UIRenderer
{
  public:
    UIRenderer(EngineWindow* window, VulkanDriver *driver);
    void init();
    void renderUI();
    void cleanup();

  private:
    EngineWindow *m_window;
    VulkanDriver *m_driver;
};