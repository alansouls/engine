#pragma once

#include "../EngineAPI.h"
#include "GameObject.h"
#include "components/ComponentType.h"

// C-style API for interop with C#
extern "C"
{
    // Get transform pointer from GameObject
    SSGE_API auto GameObject_GetTransform(SSGE::GameObject *gameObject) -> Transform *;

    SSGE_API auto GameObject_GetComponent(SSGE::GameObject *gameObject, SSGE::ComponentType componentType)
        -> SSGE::Component *;
}
