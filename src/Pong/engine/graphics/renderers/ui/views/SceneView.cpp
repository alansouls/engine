
#include "SceneView.h"
#include "imgui.h"

SceneView::SceneView(SceneRenderer *sceneRenderer) : UIView(), m_sceneRenderer(sceneRenderer)
{
    m_sceneRenderer->resize(m_width - 30, m_height - 30);
}

auto SceneView::render(const uint32_t currentImage) -> void
{
    ImGui::Begin("Scene");

    auto image = m_sceneRenderer->render(currentImage);

    ImGui::Image(image->getUITexture(), ImVec2(image->getWidth(), image->getHeight()));

    ImGui::End();
}
