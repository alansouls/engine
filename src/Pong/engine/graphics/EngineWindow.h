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

    auto initForUI() const -> void;

    static auto cleanupForUI() -> void;

    static auto beginUIFrame() -> void;

    static auto endUIFrame() -> void;

  private:
    GLFWwindow *m_window;
};