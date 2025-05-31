
#include "SceneView.h"
#include "imgui.h"

SceneView::SceneView(SceneRenderer *sceneRenderer) : UIView(), m_sceneRenderer(sceneRenderer)
{
    m_width = 500;
    m_height = 500;
    m_sceneRenderer->resize(m_width, m_height);
}

auto SceneView::render(const uint32_t currentImage) -> void
{
    ImGui::Begin("Scene");

    auto size = ImGui::GetContentRegionAvail();

    if ((static_cast<uint32_t>(size.x) != m_width || static_cast<uint32_t>(size.y) != m_height) && size.x > 0 &&
        size.y > 0)
    {
        m_width = static_cast<uint32_t>(size.x);
        m_height = static_cast<uint32_t>(size.y);
        m_sceneRenderer->resize(m_width, m_height);
    }

    const auto image = m_sceneRenderer->render(currentImage);

    ImGui::Image(image->getUITexture(),
                 ImVec2(static_cast<float>(image->getWidth()), static_cast<float>(image->getHeight())));

    ImGui::End();
}
