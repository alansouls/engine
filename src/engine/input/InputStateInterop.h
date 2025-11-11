#pragma once

#include "../EngineAPI.h"
#include "InputState.h"

// C-style API for interop with C#
extern "C"
{
    // Get key state for a specific key
    SSGE_API auto InputState_GetKeyState(const SSGE::InputState *inputState, int key) -> int;

    // Get mouse button state
    SSGE_API auto InputState_GetMouseButtonState(const SSGE::InputState *inputState, int button) -> int;

    // Get mouse position
    SSGE_API auto InputState_GetMousePositionX(const SSGE::InputState *inputState) -> double;
    SSGE_API auto InputState_GetMousePositionY(const SSGE::InputState *inputState) -> double;

    // Get mouse delta
    SSGE_API auto InputState_GetMouseDeltaX(const SSGE::InputState *inputState) -> double;
    SSGE_API auto InputState_GetMouseDeltaY(const SSGE::InputState *inputState) -> double;

    // Get mouse scroll
    SSGE_API auto InputState_GetMouseScrollX(const SSGE::InputState *inputState) -> double;
    SSGE_API auto InputState_GetMouseScrollY(const SSGE::InputState *inputState) -> double;
}
