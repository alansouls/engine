#pragma once

#include "../../EngineAPI.h"
#include "Transform.h"

// C-style API for interop with C#
extern "C"
{
    // Create and destroy transform
    SSGE_API auto Transform_Create() -> Transform*;
    SSGE_API auto Transform_Destroy(Transform* transform) -> void;

    // Transform operations
    SSGE_API auto Transform_Translate(Transform* transform, float x, float y, float z) -> void;
    SSGE_API auto Transform_Scale(Transform* transform, float x, float y, float z) -> void;
    SSGE_API auto Transform_Rotate(Transform* transform, float angle, float axisX, float axisY, float axisZ) -> void;

    // Get transform data
    SSGE_API auto Transform_GetPositionX(const Transform* transform) -> float;
    SSGE_API auto Transform_GetPositionY(const Transform* transform) -> float;
    SSGE_API auto Transform_GetPositionZ(const Transform* transform) -> float;

    // Get matrix elements (column-major order)
    SSGE_API auto Transform_GetMatrixElement(const Transform* transform, int row, int col) -> float;
}
