#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Transform
{
  public:
    Transform();

    auto translate(const glm::vec3 &translation) -> void;

    auto rotate(float angle, const glm::vec3 &axis) -> void;

    [[nodiscard]] auto getMatrix() const -> const glm::mat4&;

    [[nodiscard]] auto getPosition() const -> const glm::vec3&;

    [[nodiscard]] auto getScale() const -> const glm::vec3&;

    [[nodiscard]] auto getRotation() const -> const glm::vec3&;

    auto setPosition(const glm::vec3 &position) -> void;
    auto setScale(const glm::vec3 &scale) -> void;
    auto setRotation(const glm::vec3 &rotation) -> void;

  private:
    glm::mat4 m_matrix;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotation;

    auto updateMatrix() -> void;
};