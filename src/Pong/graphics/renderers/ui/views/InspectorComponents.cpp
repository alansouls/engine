#include "InspectorView.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "scenes/ComponentField.h"
#include "scenes/Game.h"
#include <glm/trigonometric.hpp>

namespace SSGE
{

auto InspectorView::InspectorComponents::GameObjectComponents(GameObject *gameObject) -> void
{
    assert(gameObject);

    ImGui::SeparatorText(std::format("{}", gameObject->getName()).c_str());

    ImGui::PushID(0);
    TransformComponent(gameObject);
    ImGui::PopID();

    int i = 1;
    for (auto component : gameObject->components())
    {
        ImGui::PushID(i++);
        GenericComponent(gameObject, component);
        ImGui::PopID();
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

    if (!game->isStarted())
    {
        Transform &currentTransform = gameObject->getTransform();
        currentTransform = transform;
    }
}

auto InspectorView::InspectorComponents::FieldInputInt(TypedComponentField<int> *field) -> void
{
    int originalValue = field->currentValue();
    int value = originalValue;
    ImGui::InputInt(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}

void InspectorView::InspectorComponents::FieldInputFloat(TypedComponentField<float> *field)
{
    float originalValue = field->currentValue();
    float value = originalValue;
    ImGui::InputFloat(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}
void InspectorView::InspectorComponents::FieldInputText(TypedComponentField<std::string> *field)
{
    const std::string &originalValue = field->currentValue();
    std::string value = originalValue;
    ImGui::InputText(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}
void InspectorView::InspectorComponents::FieldInputVec3(TypedComponentField<glm::vec3> *field)
{
    glm::vec3 originalValue = field->currentValue();
    glm::vec3 value = originalValue;
    ImGui::InputFloat3(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}

void InspectorView::InspectorComponents::FieldInputBool(TypedComponentField<bool> *field)
{
    bool originalValue = field->currentValue();
    bool value = originalValue;
    ImGui::Checkbox(field->name().c_str(), &value);
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}

void InspectorView::InspectorComponents::FieldInputVec2(TypedComponentField<glm::vec2> *field)
{
    glm::vec2 originalValue = field->currentValue();
    glm::vec2 value = originalValue;
    ImGui::InputFloat2(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}

void InspectorView::InspectorComponents::FieldInputVec4(TypedComponentField<glm::vec4> *field)
{
    glm::vec4 originalValue = field->currentValue();
    glm::vec4 value = originalValue;
    ImGui::InputFloat4(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setCurrentValue(value);
    }
}

void InspectorView::InspectorComponents::FieldInputColor(TypedComponentField<glm::vec4> *field)
{
    glm::vec4 originalValue = field->currentValue();
    glm::vec4 value = originalValue;
    ImGui::ColorEdit4(field->name().c_str(), reinterpret_cast<float *>(&value));
    if (value != originalValue)
    {
        field->setCurrentValue(value);
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
            FieldInputInt(reinterpret_cast<TypedComponentField<int> *>(field));
            break;
        case ComponentField::Float:
            FieldInputFloat(reinterpret_cast<TypedComponentField<float> *>(field));
            break;
        case ComponentField::String:
            FieldInputText(reinterpret_cast<TypedComponentField<std::string> *>(field));
            break;
        case ComponentField::Bool:
            FieldInputBool(reinterpret_cast<TypedComponentField<bool> *>(field));
            break;
        case ComponentField::Vec2:
            FieldInputVec2(reinterpret_cast<TypedComponentField<glm::vec2> *>(field));
            break;
        case ComponentField::Vec3:
            FieldInputVec3(reinterpret_cast<TypedComponentField<glm::vec3> *>(field));
            break;
        case ComponentField::Vec4:
            FieldInputVec4(reinterpret_cast<TypedComponentField<glm::vec4> *>(field));
            break;
        case ComponentField::Color:
            FieldInputColor(reinterpret_cast<TypedComponentField<glm::vec4> *>(field));
            break;
        default:
            throw std::runtime_error("Unknown component field type");
        }
    }
}

} // namespace SSGE
