#include "QuadRendererComponent.h"

#include "engine/scenes/GameObject.h"

#include <glm/ext/matrix_transform.hpp>

#include "scenes/ComponentField.h"

namespace SSGE
{
const std::string QuadRendererComponent::TypeName = "QuadRendererComponent";

QuadRendererComponent::QuadRendererComponent(GameObject *gameObject)
    : RendererComponent(gameObject, createItem(), TypeName), m_topLeft(1.0f, 1.0f), m_width(100.0f), m_height(100.0f),
      m_fillColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    bindFields();
}

auto QuadRendererComponent::bindFields() -> void
{
    m_fields.push_back(
        std::make_unique<ComponentField>("Top Left", "1.0|1.0", ComponentField::FieldType::Vec2, &m_topLeft));
    m_fields.push_back(std::make_unique<ComponentField>("Width", "100.0", ComponentField::FieldType::Float, &m_width));
    m_fields.push_back(
        std::make_unique<ComponentField>("Height", "100.0", ComponentField::FieldType::Float, &m_height));
    m_fields.push_back(std::make_unique<ComponentField>("Fill Color", "1.0|1.0|1.0|1.0",
                                                        ComponentField::FieldType::Color, &m_fillColor));
}

auto QuadRendererComponent::setTopLeft(const glm::vec2 topLeft) -> void
{
    m_topLeft = topLeft;
    // TODO get global position from game object transform
}

auto QuadRendererComponent::width() const -> float
{
    return m_width;
}

auto QuadRendererComponent::setWidth(float width) -> void
{
    m_width = width;
    auto item = dynamic_cast<RectangleItem *>(m_item.get());
    item->setWidth(width);
}

auto QuadRendererComponent::height() const -> float
{
    return m_height;
}

auto QuadRendererComponent::setHeight(float height) -> void
{
    m_height = height;
    auto item = dynamic_cast<RectangleItem *>(m_item.get());
    item->setHeight(height);
}

auto QuadRendererComponent::fillColor() const -> const glm::vec4 &
{
    return m_fillColor;
}

auto QuadRendererComponent::setFillColor(glm::vec4 fillColor) -> void
{
    m_fillColor = fillColor;
    auto item = dynamic_cast<RectangleItem *>(m_item.get());
    item->setFillColor(fillColor);
}

auto QuadRendererComponent::createItem() -> std::unique_ptr<RectangleItem>
{
    m_topLeft = glm::vec2(1.0f, 1.0f);
    m_width = 100.0f;
    m_height = 100.0f;
    m_fillColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    return std::make_unique<RectangleItem>(m_topLeft, m_width, m_height, m_fillColor);
}
} // namespace SSGE

extern "C"
{
    SSGE_API auto QuadRendererComponent_Create(SSGE::GameObject *gameObject) -> SSGE::QuadRendererComponent *
    {
        auto &component = gameObject->addComponent<SSGE::QuadRendererComponent>(gameObject);
        return &component;
    }

    SSGE_API auto QuadRendererComponent_GetWidth(SSGE::QuadRendererComponent *component) -> float
    {
        return component->width();
    }

    SSGE_API auto QuadRendererComponent_SetWidth(SSGE::QuadRendererComponent *component, float width) -> void
    {
        component->setWidth(width);
    }

    SSGE_API auto QuadRendererComponent_GetHeight(SSGE::QuadRendererComponent *component) -> float
    {
        return component->height();
    }

    SSGE_API auto QuadRendererComponent_SetHeight(SSGE::QuadRendererComponent *component, float height) -> void
    {
        component->setHeight(height);
    }

    SSGE_API auto QuadRendererComponent_GetFillColor(SSGE::QuadRendererComponent *component, float *colorArray) -> void
    {
        auto &fillColor = component->fillColor();
        colorArray[0] = fillColor.r;
        colorArray[1] = fillColor.g;
        colorArray[2] = fillColor.b;
        colorArray[3] = fillColor.a;
    }

    SSGE_API auto QuadRendererComponent_SetFillColor(SSGE::QuadRendererComponent *component, float *colorArray) -> void
    {
        component->setFillColor({
            colorArray[0],
            colorArray[1],
            colorArray[2],
            colorArray[3],
        });
    }
}
