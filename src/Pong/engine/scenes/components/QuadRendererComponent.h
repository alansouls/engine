#pragma once
#include "RendererComponent.h"
#include "engine/graphics/renderers/scene/RectangleItem.h"

namespace SSGE
{
class QuadRendererComponent final : public RendererComponent
{
  public:
    explicit QuadRendererComponent(GameObject *gameObject);
    ~QuadRendererComponent() override = default;

    auto setTopLeft(glm::vec2 topLeft) -> void;
    auto setWidth(float width) -> void;
    auto setHeight(float height) -> void;
    auto setFillColor(glm::vec4 fillColor) -> void;

    static const std::string TypeName;

  private:
    std::unique_ptr<RectangleItem> m_item;
    glm::vec2 m_topLeft;
    float m_width;
    float m_height;
    glm::vec4 m_fillColor;

    auto createItem() -> std::unique_ptr<RectangleItem>;
};

} // namespace SSGE
