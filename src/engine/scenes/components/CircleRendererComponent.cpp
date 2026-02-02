#include "CircleRendererComponent.h"

#include "scenes/ComponentField.h"
#include "scenes/GameObject.h"

namespace SSGE
{
const std::string CircleRendererComponent::TypeName = "CircleRendererComponent";

CircleRendererComponent::CircleRendererComponent(GameObject *gameObject)
    : RendererComponent(gameObject, createItem(), TypeName), m_center(0.0f, 0.0f), m_radius(1.0f),
      m_fillColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    bindFields();
}

auto CircleRendererComponent::center() const -> const glm::vec2 &
{
    return m_center;
}

auto CircleRendererComponent::setCenter(const glm::vec2 &center) -> void
{
    m_center = center;
    auto item = dynamic_cast<CircleItem *>(m_item.get());
    item->setCircle(m_center, m_radius);
}

auto CircleRendererComponent::radius() const -> float
{
    return m_radius;
}

auto CircleRendererComponent::setRadius(float radius) -> void
{
    m_radius = radius;
    auto item = dynamic_cast<CircleItem *>(m_item.get());
    item->setCircle(m_center, m_radius);
}

auto CircleRendererComponent::fillColor() const -> const glm::vec4 &
{
    return m_fillColor;
}

auto CircleRendererComponent::setFillColor(const glm::vec4 &fillColor) -> void
{
    m_fillColor = fillColor;
    auto item = dynamic_cast<CircleItem *>(m_item.get());
    item->setFillColor(m_fillColor);
}

auto CircleRendererComponent::createItem() -> std::unique_ptr<CircleItem>
{
    return std::make_unique<CircleItem>(m_center, m_radius, m_fillColor);
}

auto CircleRendererComponent::bindFields() -> void
{
    m_fields.push_back(
        std::make_unique<ComponentField>("Center", "0.0|0.0", ComponentField::FieldType::Vec2, &m_center));
    m_fields.push_back(std::make_unique<ComponentField>("Radius", "1.0", ComponentField::FieldType::Float, &m_radius));
    m_fields.push_back(std::make_unique<ComponentField>("Fill Color", "1.0|1.0|1.0|1.0",
                                                        ComponentField::FieldType::Color, &m_fillColor));
}
} // namespace SSGE

extern "C"
{
    auto CircleRendererComponent_Create(SSGE::GameObject *gameObject) -> SSGE::CircleRendererComponent *
    {
        auto &component = gameObject->addComponent<SSGE::CircleRendererComponent>(gameObject);
        return &component;
    }

    auto CircleRendererComponent_GetCenter(SSGE::CircleRendererComponent *component, float *centerArray) -> void
    {
        auto &center = component->center();
        centerArray[0] = center.x;
        centerArray[1] = center.y;
    }

    auto CircleRendererComponent_SetCenter(SSGE::CircleRendererComponent *component, float *centerArray) -> void
    {
        component->setCenter({centerArray[0], centerArray[1]});
    }

    auto CircleRendererComponent_GetRadius(SSGE::CircleRendererComponent *component) -> float
    {
        return component->radius();
    }

    auto CircleRendererComponent_SetRadius(SSGE::CircleRendererComponent *component, float radius) -> void
    {
        component->setRadius(radius);
    }

    auto CircleRendererComponent_GetFillColor(SSGE::CircleRendererComponent *component, float *colorArray) -> void
    {
        auto &fillColor = component->fillColor();
        colorArray[0] = fillColor.r;
        colorArray[1] = fillColor.g;
        colorArray[2] = fillColor.b;
        colorArray[3] = fillColor.a;
    }

    auto CircleRendererComponent_SetFillColor(SSGE::CircleRendererComponent *component, float *colorArray) -> void
    {
        component->setFillColor({
            colorArray[0],
            colorArray[1],
            colorArray[2],
            colorArray[3],
        });
    }
}
