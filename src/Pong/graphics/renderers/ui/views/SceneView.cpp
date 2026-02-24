
#include "SceneView.h"
#include "../../scene/EditorSceneRenderer.h"
#include "imgui.h"
#include "scenes/Game.h"

SceneView::SceneView(const std::string &name, EditorSceneRenderer *sceneRenderer, bool forwardGameInput)
    : UIView(name, nullptr), m_sceneRenderer(sceneRenderer), m_forwardGameInput(forwardGameInput)
{
    m_width = 500;
    m_height = 500;
    m_open = true;
    m_sceneRenderer->resize(m_width, m_height);
}

auto SceneView::render(const uint32_t currentImage) -> void
{
    ImGui::Begin(m_name.data(), &m_open);

    if (m_forwardGameInput)
    {
        Game::getInstance()->setGameInputEnabled(ImGui::IsWindowFocused());
    }

    auto size = ImGui::GetContentRegionAvail();

    if ((static_cast<uint32_t>(size.x) != m_width || static_cast<uint32_t>(size.y) != m_height) && size.x > 0 &&
        size.y > 0)
    {
        m_width = static_cast<uint32_t>(size.x);
        m_height = static_cast<uint32_t>(size.y);
        m_sceneRenderer->resize(m_width, m_height);
    }

    const auto image = m_sceneRenderer->render(currentImage);

    if (image->getIsReady())
    {
        ImGui::Image(image->getUITexture(),
                     ImVec2(static_cast<float>(image->getWidth()), static_cast<float>(image->getHeight())));
    }

    ImGui::End();
}
