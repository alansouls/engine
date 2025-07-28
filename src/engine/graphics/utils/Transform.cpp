//
// Created by maiaa on 04/06/2025.
//

#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
Transform::Transform() : m_matrix(1.0f)
{
}

auto Transform::translate(const glm::vec3 &translation) -> void
{
    m_matrix = glm::translate(m_matrix, translation);
}

auto Transform::scale(const glm::vec3 &scale) -> void
{
    m_matrix = glm::scale(m_matrix, scale);
}

auto Transform::rotate(float angle, const glm::vec3 &axis) -> void
{
    m_matrix = glm::rotate(m_matrix, angle, axis);
}

auto Transform::getMatrix() const -> const glm::mat4 &
{
    return m_matrix;
}

auto Transform::position() const -> glm::vec3
{
    return {m_matrix[3]};
}