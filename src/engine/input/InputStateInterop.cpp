#include "InputStateInterop.h"

extern "C"
{
    SSGE_API auto InputState_GetKeyState(const SSGE::InputState *inputState, int key) -> int
    {
        if (inputState == nullptr)
        {
            return static_cast<int>(SSGE::KeyState::Released);
        }
        return static_cast<int>(inputState->getKeyState(key));
    }

    SSGE_API auto InputState_GetMouseButtonState(const SSGE::InputState *inputState, int button) -> int
    {
        if (inputState == nullptr || button < 0 || button >= static_cast<int>(SSGE::MouseButton::Count))
        {
            return static_cast<int>(SSGE::KeyState::Released);
        }

        auto mouseButton = static_cast<SSGE::MouseButton>(button);
        if (inputState->isMouseButtonPressed(mouseButton))
        {
            return static_cast<int>(SSGE::KeyState::Pressed);
        }
        else if (inputState->isMouseButtonHeld(mouseButton))
        {
            return static_cast<int>(SSGE::KeyState::Held);
        }
        return static_cast<int>(SSGE::KeyState::Released);
    }

    SSGE_API auto InputState_GetMousePositionX(const SSGE::InputState *inputState) -> double
    {
        if (inputState == nullptr)
        {
            return 0.0;
        }
        return inputState->getMousePosition().x;
    }

    SSGE_API auto InputState_GetMousePositionY(const SSGE::InputState *inputState) -> double
    {
        if (inputState == nullptr)
        {
            return 0.0;
        }
        return inputState->getMousePosition().y;
    }

    SSGE_API auto InputState_GetMouseDeltaX(const SSGE::InputState *inputState) -> double
    {
        if (inputState == nullptr)
        {
            return 0.0;
        }
        return inputState->getMouseDelta().x;
    }

    SSGE_API auto InputState_GetMouseDeltaY(const SSGE::InputState *inputState) -> double
    {
        if (inputState == nullptr)
        {
            return 0.0;
        }
        return inputState->getMouseDelta().y;
    }

    SSGE_API auto InputState_GetMouseScrollX(const SSGE::InputState *inputState) -> double
    {
        if (inputState == nullptr)
        {
            return 0.0;
        }
        return inputState->getMouseScroll().xOffset;
    }

    SSGE_API auto InputState_GetMouseScrollY(const SSGE::InputState *inputState) -> double
    {
        if (inputState == nullptr)
        {
            return 0.0;
        }
        return inputState->getMouseScroll().yOffset;
    }
}
