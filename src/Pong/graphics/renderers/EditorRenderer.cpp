#include "EditorRenderer.h"

#include "scene/EditorSceneRenderer.h"

EditorRenderer::EditorRenderer(SSGE::Editor::EngineGame *game, EngineWindow *mainWindow,
                               const SSGE::RendererOptions &options)
    : Renderer(mainWindow, options), m_drawData(nullptr)
{
    m_editorSceneRenderer = std::make_unique<EditorSceneRenderer>(m_driver.get(), 0, 0);
    m_gameSceneRenderer = std::make_unique<EditorSceneRenderer>(m_driver.get(), 0, 0);
    m_sceneRenderers = {m_editorSceneRenderer.get(), m_gameSceneRenderer.get()};
    m_uiRenderer = std::make_unique<UIRenderer>(game, mainWindow, m_driver.get());
    m_uiRenderer->init(m_editorSceneRenderer.get(), m_gameSceneRenderer.get());
}

EditorRenderer::~EditorRenderer()
{
    m_driver->waitIdle();
}

auto EditorRenderer::drawFrame(uint32_t currentFrame) -> void
{
    m_driver->drawFrame(currentFrame, m_drawData);
}

auto EditorRenderer::getSceneWidth() const -> uint32_t
{
    return m_editorSceneRenderer->getWidth();
}

auto EditorRenderer::getSceneHeight() const -> uint32_t
{
    return m_editorSceneRenderer->getHeight();
}

auto EditorRenderer::preRender(uint32_t currentFrame) -> void
{
    m_drawData = m_uiRenderer->renderUI(currentFrame);
}
