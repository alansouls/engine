#pragma once
#include <string>

namespace SSGE
{
class GameObject;
class Component
{
  public:
    explicit Component(const std::string &name, GameObject* gameObject);
    virtual ~Component() = default;

    [[nodiscard]] auto name() const -> const std::string &
    {
        return m_name;
    }

    virtual auto init() -> void = 0;
    virtual auto update() -> void = 0;

    [[nodiscard]] auto gameObject() const -> GameObject &
    {
        return *m_gameObject;
    }

  private:
    std::string m_name;
    GameObject* m_gameObject;
};
} // namespace SSGE