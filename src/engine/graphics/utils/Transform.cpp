//
// Created by maiaa on 04/06/2025.
//

#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
Transform::Transform() : m_matrix(1.0f), m_position(0.0f), m_scale(1.0f)
{
}

auto Transform::translate(const glm::vec3 &translation) -> void
{
    m_position = translation;
    updateMatrix();
}

auto Transform::scale(const glm::vec3 &scale) -> void
{
    m_scale = scale;
    updateMatrix();
}

auto Transform::rotate(float angle, const glm::vec3 &axis) -> void
{
    // m_matrix = glm::rotate(m_matrix, angle, axis);
}

auto Transform::getMatrix() const -> const glm::mat4 &
{
    return m_matrix;
}

auto Transform::position() const -> glm::vec3
{
    return m_position;
}

auto Transform::getScale() const -> glm::vec3
{
    return m_scale;
}

auto Transform::updateMatrix() -> void
{
    m_matrix = glm::translate(glm::mat4(1.0f), m_position);
    m_matrix = glm::scale(m_matrix, m_scale);
}