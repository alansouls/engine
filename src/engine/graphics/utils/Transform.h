#pragma once
#include <glm/glm.hpp>

class Transform
{
  public:
    Transform();

    auto translate(const glm::vec3 &translation) -> void;

    auto scale(const glm::vec3 &scale) -> void;

    auto rotate(float angle, const glm::vec3 &axis) -> void;

    [[nodiscard]] auto getMatrix() const -> const glm::mat4 &;

    [[nodiscard]] auto position() const -> glm::vec3;

  private:
    glm::mat4 m_matrix;
};