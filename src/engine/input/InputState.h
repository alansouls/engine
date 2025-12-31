#pragma once

#include <array>
#include <unordered_map>

namespace SSGE
{

enum class KeyState
{
    None,
    Released,
    Pressed,
    Held
};

enum class MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Count = 5
};

struct MousePosition
{
    double x = 0.0;
    double y = 0.0;
};

struct MouseScroll
{
    double xOffset = 0.0;
    double yOffset = 0.0;
};

class InputState
{
  public:
    InputState() = default;

    // Key state queries
    [[nodiscard]] auto isKeyPressed(int key) const -> bool;
    [[nodiscard]] auto isKeyReleased(int key) const -> bool;
    [[nodiscard]] auto isKeyHeld(int key) const -> bool;
    [[nodiscard]] auto getKeyState(int key) const -> KeyState;

    // Mouse button state queries
    [[nodiscard]] auto isMouseButtonPressed(MouseButton button) const -> bool;
    [[nodiscard]] auto isMouseButtonReleased(MouseButton button) const -> bool;
    [[nodiscard]] auto isMouseButtonHeld(MouseButton button) const -> bool;

    // Mouse position
    [[nodiscard]] auto getMousePosition() const -> const MousePosition &;
    [[nodiscard]] auto getMouseDelta() const -> MousePosition;

    // Mouse scroll
    [[nodiscard]] auto getMouseScroll() const -> const MouseScroll &;

    // Internal update methods (used by InputManager)
    auto updateKeyState(int key, KeyState state) -> void;
    auto updateMouseButtonState(MouseButton button, KeyState state) -> void;
    auto updateMousePosition(double x, double y) -> void;
    auto updateMouseScroll(double xOffset, double yOffset) -> void;

    // Frame update - transitions Pressed->Held and Released->Released (clears one-frame states)
    auto beginFrame() -> void;

  private:
    std::unordered_map<int, KeyState> m_keyStates;
    std::unordered_map<int, KeyState> m_keyStateChanges;
    std::array<KeyState, static_cast<size_t>(MouseButton::Count)> m_mouseButtonStates{KeyState::Released};
    
    MousePosition m_mousePosition{};
    MousePosition m_previousMousePosition{};
    MouseScroll m_mouseScroll{};
};

} // namespace SSGE
