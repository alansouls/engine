
#include "game/PongGame.h"
#include <cstdlib>
#include <iostream>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

#ifdef NDEBUG
constexpr bool debugModeOn = false;
#else
constexpr bool debugModeOn = true;
#endif

class GameApplication
{
  public:
    void run()
    {
        initWindow();
        m_game = new PongGame(debugModeOn, m_window);
        mainLoop();
        cleanup();
    }

  private:
    void initWindow()
    {
        m_window = new EngineWindow(EngineWindow::EngineWindowProperties::mainWindowProperties("Pong"));
    }

    void mainLoop()
    {
        std::cout << "Entering main loop..." << std::endl;

        m_game->setup();
        m_game->run();

        std::cout << "Finshed!" << std::endl;
    }

    void cleanup()
    {
        std::cout << "Cleaning up resources..." << std::endl;

        delete m_game;

        delete m_window;

        glfwTerminate();

        std::cout << "Finshed!" << std::endl;
    }

    EngineWindow *m_window = nullptr;
    Game *m_game = nullptr;
};

int main()
{
    try
    {
        std::cin.get();
        GameApplication app;
        app.run();
    }
    catch (const std::exception &e)
    {
        throw;
    }

    return EXIT_SUCCESS;
}
