//
// Created by Alan Maia on 01/02/2026.
//

#include "InspectorView.h"

#include "imgui_internal.h"

namespace SSGE
{
    InspectorView::InspectorView(UIMessenger* messenger) :
        UIView("Inspector", messenger), m_selectedGameObject(nullptr)
    {
        messenger->connect("SelectedGameObjectChanged", [this](void* data)
        {
            m_selectedGameObject = static_cast<SSGE::GameObject*>(data);
        });
    }

    auto InspectorView::render(uint32_t currentImage) -> void
    {
        ImGui::Begin("Inspector", &m_open);

        if (m_selectedGameObject)
        {
            ImGui::Text("%s", m_selectedGameObject->getName().c_str());



            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                float position[3] = {0.0f};
                ImGui::InputFloat3("Position", position);
                ImGui::SeparatorText("Scale");
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
} // SSGE
