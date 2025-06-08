#pragma once

#include "glm/vec2.hpp"

namespace SSGE
{
class Collider;
struct CollisionInfo
{
    Collider *collider;
    Collider *other;
    glm::vec2 contactPoint;
    bool entered;
};
} // namespace SSGE