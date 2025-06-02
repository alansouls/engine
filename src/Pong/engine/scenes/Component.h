#pragma once
#include <string>

namespace SSGE
{
class GameObject;
class Component
{
  public:
    explicit Component(const std::string &name);
    virtual ~Component() = default;

    [[nodiscard]] auto name() const -> const std::string &
    {
        return m_name;
    }

    virtual auto init() -> void = 0;
    virtual auto update() -> void = 0;
    virtual auto gameObject() -> GameObject & = 0;

  private:
    std::string m_name;
};
} // namespace SSGE