#include "Collider.h"

extern "C"
{
    SSGE_API auto Collider_GetLayer(SSGE::Collider *collider) -> const char *
    {
        return collider->getLayer().c_str();
    }

    SSGE_API auto Collider_SetLayer(SSGE::Collider *collider, const char *layer) -> void
    {
        collider->setLayer(layer);
    }

    SSGE_API auto Collider_SetCollidesWith(SSGE::Collider *collider, const char **layers, int layersLength) -> void
    {
        std::vector<std::string> layersVector;
        layersVector.reserve(layersLength);
        for (int i = 0; i < layersLength; i++)
        {
            layersVector.emplace_back(layers[i]);
        }
        collider->setCollidesWith(layersVector);
    }
}

