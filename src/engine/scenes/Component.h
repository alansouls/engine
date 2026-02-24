#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ComponentField.h"

namespace SSGE
{
class ComponentField;
class GameObject;

class Component
{
  public:
    explicit Component(std::string name, std::string displayName, GameObject *gameObject)
        : m_name(std::move(name)), m_displayName(std::move(displayName)), m_gameObject(gameObject)
    {
    }

    virtual ~Component() = default;

    [[nodiscard]] auto name() const -> const std::string &
    {
        return m_name;
    }

    virtual auto init() -> void = 0;
    virtual auto update() -> void = 0;

    auto applyInitialValues() const -> void
    {
        for (auto &field : m_fields)
        {
            field->applyInitialValue();
        }
    }

    [[nodiscard]] auto gameObject() const -> GameObject *
    {
        return m_gameObject;
    }

    [[nodiscard]] auto getComponentFields() const -> std::vector<ComponentField *>
    {
        std::vector<ComponentField *> componentFields;
        componentFields.reserve(m_fields.size());

        for (auto &field : m_fields)
            componentFields.push_back(field.get());

        return componentFields;
    }

    [[nodiscard]] auto displayName() const -> const std::string &
    {
        return m_displayName;
    }

  protected:
    std::vector<std::unique_ptr<ComponentField>> m_fields;

  private:
    std::string m_name;
    std::string m_displayName;
    GameObject *m_gameObject;
};
} // namespace SSGE
