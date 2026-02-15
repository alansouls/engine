#pragma once
#include "EngineAPI.h"
#include "RendererComponent.h"
#include "engine/graphics/renderers/scene/RectangleItem.h"

namespace SSGE
{
class QuadRendererComponent final : public RendererComponent
{
  public:
    explicit QuadRendererComponent(GameObject *gameObject);
    ~QuadRendererComponent() override = default;

    auto topLeft() const -> glm::vec2;
    auto setTopLeft(glm::vec2 topLeft) -> void;
    [[nodiscard]] auto width() const -> float;
    auto setWidth(float width) -> void;
    [[nodiscard]] auto height() const -> float;
    auto setHeight(float height) -> void;
    [[nodiscard]] auto fillColor() const -> const glm::vec4 &;
    auto setFillColor(glm::vec4 fillColor) -> void;

    static const std::string TypeName;

  private:
    glm::vec2 m_topLeft;
    float m_width;
    float m_height;
    glm::vec4 m_fillColor;

    auto createItem() -> std::unique_ptr<RectangleItem>;
    auto bindFields() -> void;
};
} // namespace SSGE

extern "C"
{
    SSGE_API auto QuadRendererComponent_Create(SSGE::GameObject *gameObject) -> SSGE::QuadRendererComponent *;

    SSGE_API auto QuadRendererComponent_GetWidth(SSGE::QuadRendererComponent *component) -> float;

    SSGE_API auto QuadRendererComponent_SetWidth(SSGE::QuadRendererComponent *component, float width) -> void;

    SSGE_API auto QuadRendererComponent_GetHeight(SSGE::QuadRendererComponent *component) -> float;

    SSGE_API auto QuadRendererComponent_SetHeight(SSGE::QuadRendererComponent *component, float height) -> void;

    SSGE_API auto QuadRendererComponent_GetFillColor(SSGE::QuadRendererComponent *component, float *colorArray) -> void;

    SSGE_API auto QuadRendererComponent_SetFillColor(SSGE::QuadRendererComponent *component, float *colorArray) -> void;
}
