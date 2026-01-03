#include "GameObjectInterop.h"

#include "components/CircleRendererComponent.h"
#include "components/QuadRendererComponent.h"

extern "C"
{
    SSGE_API auto GameObject_GetTransform(SSGE::GameObject *gameObject) -> Transform *
    {
        if (gameObject == nullptr)
        {
            return nullptr;
        }
        return &gameObject->getTransform();
    }

    auto GameObject_GetComponent(SSGE::GameObject *gameObject, SSGE::ComponentType componentType) -> SSGE::Component *
    {
        switch (componentType)
        {
        case SSGE::ComponentType::QuadRenderer: {
            auto component = gameObject->getComponent<SSGE::QuadRendererComponent>();
            if (component.has_value())
            {
                return *component;
            }
            return nullptr;
        }
        case SSGE::ComponentType::CircleRenderer: {
            auto component = gameObject->getComponent<SSGE::CircleRendererComponent>();
            if (component.has_value())
            {
                return *component;
            }
            return nullptr;
        }
        default:
            return nullptr;
        }
    }
}