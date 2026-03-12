//
// Created by Alan Maia on 01/02/2026.
//

#include "InspectorView.h"
#include "imgui.h"

#include <glm/ext/scalar_constants.hpp>

namespace SSGE
{
InspectorView::InspectorView(Messenger *messenger) : UIView("Inspector", messenger), m_selectedGameObject(nullptr)
{
    m_open = true;
    messenger->connect("SelectedGameObjectChanged",
                       [this](void *data) { m_selectedGameObject = static_cast<SSGE::GameObject *>(data); });
}

auto InspectorView::render(uint32_t currentImage) -> void
{
    ImGui::Begin("Inspector", &m_open);

    if (m_selectedGameObject)
    {
        InspectorComponents::GameObjectComponents(m_selectedGameObject);
    }

    ImGui::End();
}
} // namespace SSGE
