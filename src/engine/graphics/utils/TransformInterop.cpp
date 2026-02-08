#include "TransformInterop.h"

extern "C"
{
    SSGE_API auto Transform_Create() -> Transform *
    {
        return new Transform();
    }

    SSGE_API auto Transform_Destroy(Transform *transform) -> void
    {
        delete transform;
    }

    SSGE_API auto Transform_Translate(Transform *transform, float x, float y, float z) -> void
    {
        if (transform == nullptr)
        {
            return;
        }
        transform->translate(glm::vec3(x, y, z));
    }

    SSGE_API auto Transform_Rotate(Transform *transform, float angle, float axisX, float axisY, float axisZ) -> void
    {
        if (transform == nullptr)
        {
            return;
        }
        transform->rotate(angle, glm::vec3(axisX, axisY, axisZ));
    }

    SSGE_API auto Transform_GetPositionX(const Transform *transform) -> float
    {
        if (transform == nullptr)
        {
            return 0.0f;
        }
        return transform->getPosition().x;
    }

    SSGE_API auto Transform_GetPositionY(const Transform *transform) -> float
    {
        if (transform == nullptr)
        {
            return 0.0f;
        }
        return transform->getPosition().y;
    }

    SSGE_API auto Transform_GetPositionZ(const Transform *transform) -> float
    {
        if (transform == nullptr)
        {
            return 0.0f;
        }
        return transform->getPosition().z;
    }

    SSGE_API auto Transform_GetMatrixElement(const Transform *transform, int row, int col) -> float
    {
        if (transform == nullptr || row < 0 || row >= 4 || col < 0 || col >= 4)
        {
            return 0.0f;
        }
        return transform->getMatrix()[col][row];
    }
}
