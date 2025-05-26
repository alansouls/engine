#pragma once

#include <GLFW/glfw3.h>
#include <string>

class EngineWindow
{
  public:
    struct EngineWindowProperties
    {
        int width;
        int height;
        std::string title;
        bool resizable;

        static EngineWindowProperties mainWindowProperties(const std::string &appTitle)
        {
            return {800, 600, appTitle, true};
        }
    };

    struct WindowSize
    {
        int width;
        int height;
    };

    EngineWindow(const EngineWindowProperties &properties);
    ~EngineWindow();

    auto getSize() const -> WindowSize;

    auto getWindow() -> GLFWwindow *;

    auto initForUI() -> void;

    auto cleanupForUI() -> void;

    auto beginUIFrame() -> void;

    auto endUIFrame() -> void;

  private:
    GLFWwindow *m_window;
};