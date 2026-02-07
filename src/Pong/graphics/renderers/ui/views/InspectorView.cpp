//
// Created by Alan Maia on 01/02/2026.
//

#include "InspectorView.h"

#include "imgui_internal.h"

namespace SSGE
{
InspectorView::InspectorView(UIMessenger *messenger) : UIView("Inspector", messenger), m_selectedGameObject(nullptr)
{
    messenger->connect("SelectedGameObjectChanged",
                       [this](void *data) { m_selectedGameObject = static_cast<SSGE::GameObject *>(data); });
}

auto InspectorView::render(uint32_t currentImage) -> void
{
    ImGui::Begin("Inspector", &m_open);

    if (m_selectedGameObject)
    {
        ImGui::Text("%s", m_selectedGameObject->getName().c_str());

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Transform &transform = m_selectedGameObject->getTransform();

            float position[3] = {transform.position()[0], transform.position()[1], transform.position()[2]};
            ImGui::InputFloat3("Position", position);

            if (position[0] != transform.position()[0] || position[1] != transform.position()[1] || position[2] != transform.position()[2])
            {
                transform.translate(glm::vec3(position[0], position[1], position[2]));
            }

            float scale[3] = {transform.getScale()[0], transform.getScale()[1], transform.getScale()[2]};
            ImGui::InputFloat3("Scale", scale);

            if (scale[0] != transform.getScale()[0] || scale[1] != transform.getScale()[1] || scale[2] != transform.getScale()[2])
            {
                transform.scale(glm::vec3(scale[0], scale[1], scale[2]));
            }

            ImGui::SeparatorText("Rotation");
        }

        for (auto component : m_selectedGameObject->components())
        {
            if (ImGui::CollapsingHeader(component->name().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
            }
        }
    }

    ImGui::End();
}
} // namespace SSGE
