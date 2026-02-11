#include "InspectorView.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "scenes/Game.h"
#include <glm/trigonometric.hpp>

namespace SSGE
{

auto InspectorView::InspectorComponents::GameObjectComponents(GameObject *gameObject) -> void
{
    assert(gameObject);

    ImGui::SeparatorText(std::format("{}", gameObject->getName()).c_str());

    TransformComponent(gameObject);

    for (auto component : gameObject->components())
    {
        GenericComponent(gameObject, component);
    }
}

auto InspectorView::InspectorComponents::TransformComponent(GameObject *gameObject) -> void
{
    assert(gameObject);

    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    auto game = Game::getInstance();

    Transform &transform = game->isStarted() ? gameObject->getTransform() : gameObject->getInitialTransform();

    const glm::vec3 &originalPosition = transform.getPosition();
    glm::vec3 position = originalPosition;
    ImGui::InputFloat3("Position", reinterpret_cast<float *>(&position));

    if (position != originalPosition)
    {
        transform.setPosition(position);
    }

    const glm::vec3 &originalScale = transform.getScale();
    glm::vec3 scale = originalScale;
    ImGui::InputFloat3("Scale", reinterpret_cast<float *>(&scale));

    if (scale != originalScale)
    {
        transform.setScale(scale);
    }

    const glm::vec3 &originalRotation = transform.getRotation();
    glm::vec3 rotation = glm::degrees(originalRotation);
    ImGui::InputFloat3("Rotation", reinterpret_cast<float *>(&rotation));
    rotation = glm::radians(rotation);

    if (rotation != originalRotation)
    {
        transform.setRotation(rotation);
    }
}

auto InspectorView::InspectorComponents::FieldInputInt(ComponentField *field) -> void
{
    int originalValue = std::stoi(field->activeValue());
    int value = originalValue;
    ImGui::InputInt(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setActiveValue(std::to_string(value));
        field->applyActiveValue();
    }
}

void InspectorView::InspectorComponents::FieldInputFloat(ComponentField *field)
{
    float originalValue = std::stof(field->activeValue());
    float value = originalValue;
    ImGui::InputFloat(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setActiveValue(std::to_string(value));
        field->applyActiveValue();
    }
}
void InspectorView::InspectorComponents::FieldInputText(ComponentField *field)
{
    const std::string &originalValue = field->activeValue();
    std::string value = originalValue;
    ImGui::InputText(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setActiveValue(value);
        field->applyActiveValue();
    }
}
void InspectorView::InspectorComponents::FieldInputVec3(ComponentField *field)
{
    glm::vec3 originalValue = ComponentField::stringToVec3(field->activeValue());
    glm::vec3 value = originalValue;
    ImGui::InputFloat3(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setActiveValue(ComponentField::vec3ToString(value));
        field->applyActiveValue();
    }
}

void InspectorView::InspectorComponents::FieldInputBool(ComponentField *field)
{
    bool originalValue = field->activeValue() == "true";
    bool value = originalValue;
    ImGui::Checkbox(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setActiveValue(value ? "true" : "false");
        field->applyActiveValue();
    }
}

void InspectorView::InspectorComponents::FieldInputVec2(ComponentField *field)
{
    glm::vec2 originalValue = ComponentField::stringToVec2(field->activeValue());
    glm::vec2 value = originalValue;
    ImGui::InputFloat2(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setActiveValue(ComponentField::vec2ToString(value));
        field->applyActiveValue();
    }
}

void InspectorView::InspectorComponents::FieldInputVec4(ComponentField *field)
{
    glm::vec4 originalValue = ComponentField::stringToVec4(field->activeValue());
    glm::vec4 value = originalValue;
    ImGui::InputFloat4(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setActiveValue(ComponentField::vec4ToString(value));
        field->applyActiveValue();
    }
}

void InspectorView::InspectorComponents::FieldInputColor(ComponentField *field)
{
    glm::vec4 originalValue = ComponentField::stringToVec4(field->activeValue());
    glm::vec4 value = originalValue;
    ImGui::ColorEdit4(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setActiveValue(ComponentField::vec4ToString(value));
        field->applyActiveValue();
    }
}

auto InspectorView::InspectorComponents::GenericComponent(GameObject *gameObject, Component *component) -> void
{
    assert(gameObject && component);

    if (!ImGui::CollapsingHeader(component->name().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        return;

    for (ComponentField *field : component->getComponentFields())
    {
        switch (field->type())
        {
        case ComponentField::Int:
            FieldInputInt(field);
            break;
        case ComponentField::Float:
            FieldInputFloat(field);
            break;
        case ComponentField::String:
            FieldInputText(field);
            break;
        case ComponentField::Bool:
            FieldInputBool(field);
            break;
        case ComponentField::Vec2:
            FieldInputVec2(field);
            break;
        case ComponentField::Vec3:
            FieldInputVec3(field);
            break;
        case ComponentField::Vec4:
            FieldInputVec4(field);
            break;
        case ComponentField::Color:
            FieldInputColor(field);
            break;
        default:
            throw std::runtime_error("Unknown component field type");
        }
    }
}

} // namespace SSGE
