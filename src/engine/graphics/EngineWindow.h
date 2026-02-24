#pragma once

#include <GLFW/glfw3.h>
#include <functional>
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
        bool isMain;

        static EngineWindowProperties mainWindowProperties(const std::string &appTitle)
        {
            return {800, 600, appTitle, true, true};
        }
    };

    struct WindowSize
    {
        int width;
        int height;
    };

    explicit EngineWindow(const EngineWindowProperties &properties);
    ~EngineWindow();

    [[nodiscard]] auto getSize() const -> WindowSize;

    auto getWindow() -> GLFWwindow *;

    auto initForUI() const -> void;

    static auto cleanupForUI() -> void;

    static auto beginUIFrame() -> void;

    static auto endUIFrame() -> void;
    auto setKeyCallback(const std::function<void(int, int, int, int)> &callback) -> void;
    auto setMouseButtonCallback(const std::function<void(int, int, int)> &callback) -> void;
    auto setCursorPositionCallback(const std::function<void(double, double)> &callback) -> void;
    auto setScrollCallback(const std::function<void(double, double)> &callback) -> void;
    auto setFramebufferResizeCallback(const std::function<void(int, int)> &callback) -> void;

  private:
    GLFWwindow *m_window;

    std::function<void(int, int, int, int)> m_keyCallback;
    std::function<void(int, int, int)> m_mouseButtonCallback;
    std::function<void(double, double)> m_cursorPositionCallback;
    std::function<void(double, double)> m_scrollCallback;
    std::function<void(int, int)> m_framebufferResizeCallback;

    static auto keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) -> void;
    static auto mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) -> void;
    static auto cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) -> void;
    static auto scrollCallback(GLFWwindow *window, double xoffset, double yoffset) -> void;
    static auto framebufferResizeCallback(GLFWwindow *window, int, int) -> void;

    static EngineWindow* s_mainWindow;
};