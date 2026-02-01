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
        }

        ImGui::End();
    }
} // SSGE
