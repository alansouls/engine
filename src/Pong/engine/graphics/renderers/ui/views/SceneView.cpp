
#include "SceneView.h"
#include "imgui.h"

SceneView::SceneView(const SceneImage &image) : UIView(), m_sceneImage(image)
{
}

auto SceneView::render() -> void
{
    ImGui::Begin("Scene");

    ImGui::Image((ImTextureID)m_sceneImage.descriptorSet, ImVec2(m_sceneImage.width, m_sceneImage.height));

    ImGui::End();
}
