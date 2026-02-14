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
    m_fields.push_back(std::make_unique<TypedComponentField<glm::vec2>>(
        "Top Left", ComponentField::FieldType::Vec2, [this]() { return this->topLeft(); },
        [this](auto &vec2) { this->setTopLeft(vec2); }));
    m_fields.push_back(std::make_unique<TypedComponentField<float>>(
        "Width", ComponentField::FieldType::Float, [this]() { return this->width(); },
        [this](auto &width) { this->setWidth(width); }));
    m_fields.push_back(std::make_unique<TypedComponentField<float>>(
        "Height", ComponentField::FieldType::Float, [this]() { return this->height(); },
        [this](auto &height) { return this->setHeight(height); }));
    m_fields.push_back(std::make_unique<TypedComponentField<glm::vec4>>(
        "Fill Color", ComponentField::FieldType::Color, [this]() { return this->fillColor(); },
        [this](auto &fillColor) { this->setFillColor(fillColor); }));
}

auto QuadRendererComponent::topLeft() const -> glm::vec2
{
    return m_topLeft;
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
