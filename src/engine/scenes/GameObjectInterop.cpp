#include "GameObjectInterop.h"

extern "C"
{
    SSGE_API auto GameObject_GetTransform(SSGE::GameObject* gameObject) -> Transform*
    {
        if (gameObject == nullptr)
        {
            return nullptr;
        }
        return &gameObject->getTransform();
    }
}
