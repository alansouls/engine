#pragma once
#include "EngineAPI.h"
#include "RendererComponent.h"
#include "engine/graphics/renderers/scene/CircleItem.h"

#include <glm/glm.hpp>

namespace SSGE
{

class CircleRendererComponent final : public RendererComponent
{
  public:
    explicit CircleRendererComponent(GameObject *gameObject);
    ~CircleRendererComponent() override = default;

    [[nodiscard]] auto center() const -> const glm::vec2 &;
    auto setCenter(const glm::vec2 &center) -> void;
    [[nodiscard]] auto radius() const -> float;
    auto setRadius(float radius) -> void;
    [[nodiscard]] auto fillColor() const -> const glm::vec4 &;
    auto setFillColor(const glm::vec4 &fillColor) -> void;

    static const std::string TypeName;

  private:
    glm::vec2 m_center;
    float m_radius;
    glm::vec4 m_fillColor;

    auto createItem() -> std::unique_ptr<CircleItem>;
    auto bindFields() -> void;
};

} // namespace SSGE

extern "C"
{
    SSGE_API auto CircleRendererComponent_Create(SSGE::GameObject *gameObject) -> SSGE::CircleRendererComponent *;

    SSGE_API auto CircleRendererComponent_GetCenter(SSGE::CircleRendererComponent *component, float *centerArray)
        -> void;

    SSGE_API auto CircleRendererComponent_SetCenter(SSGE::CircleRendererComponent *component, float *centerArray)
        -> void;

    SSGE_API auto CircleRendererComponent_GetRadius(SSGE::CircleRendererComponent *component) -> float;

    SSGE_API auto CircleRendererComponent_SetRadius(SSGE::CircleRendererComponent *component, float radius) -> void;

    SSGE_API auto CircleRendererComponent_GetFillColor(SSGE::CircleRendererComponent *component, float *colorArray)
        -> void;

    SSGE_API auto CircleRendererComponent_SetFillColor(SSGE::CircleRendererComponent *component, float *colorArray)
        -> void;
}