#include "SSGEEditor.h"

namespace SSGE::Editor
{

auto SSGEEditor::run(bool debugModeOn, const std::string &dotnetProjectPath) -> void
{
    m_window = std::make_unique<EngineWindow>(EngineWindow::EngineWindowProperties::mainWindowProperties("SSGE 2D"));
    m_game = std::make_unique<EngineGame>(m_window.get(), dotnetProjectPath, "SSGEDotNet.Sample", debugModeOn);
    mainLoop();
}

auto SSGEEditor::mainLoop() const -> void
{
    std::cout << "Entering main loop..." << std::endl;

    m_game->setup();
    m_game->run();

    std::cout << "Finished!" << std::endl;
}

} // namespace SSGE::Editor
