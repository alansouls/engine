//
// Created by maiaa on 04/06/2025.
//

#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>

Transform::Transform() : m_matrix(1.0f), m_position(0.0f), m_scale(1.0f), m_rotation(0.0f)
{
}

auto Transform::translate(const glm::vec3 &translation) -> void
{
    m_position += translation;
    m_matrix = glm::translate(m_matrix, translation);
}

auto Transform::rotate(float angle, const glm::vec3 &axis) -> void
{
    m_rotation = m_rotation + axis * angle;
    m_matrix = glm::rotate(m_matrix, angle, axis);
}

auto Transform::getMatrix() const -> const glm::mat4 &
{
    return m_matrix;
}

auto Transform::getPosition() const -> const glm::vec3&
{
    return m_position;
}

auto Transform::getScale() const -> const glm::vec3&
{
    return m_scale;
}

auto Transform::getRotation() const -> const glm::vec3&
{
    return m_rotation;
}

auto Transform::setPosition(const glm::vec3 &position) -> void
{
    m_position = position;
    updateMatrix();
}

auto Transform::setScale(const glm::vec3 &scale) -> void
{
    m_scale = scale;
    updateMatrix();
}

auto Transform::setRotation(const glm::vec3 &rotation) -> void
{
    m_rotation = rotation;
    updateMatrix();
}

auto Transform::updateMatrix() -> void
{
    m_matrix = glm::translate(glm::mat4(1.0f), m_position);
    m_matrix = glm::scale(m_matrix, m_scale);
    m_matrix = glm::rotate(m_matrix, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_matrix = glm::rotate(m_matrix, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_matrix = glm::rotate(m_matrix, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}