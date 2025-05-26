
#include "game/PongGame.h"
#include <cstdlib>
#include <iostream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool debugModeOn = false;
#else
const bool debugModeOn = true;
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
        glfwInit();

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

    EngineWindow *m_window;
    Game *m_game;
};

int main()
{
    {
        GameApplication app;

        try
        {
            app.run();
        }
        catch (const std::exception &e)
        {
            throw;
        }
    }

    return EXIT_SUCCESS;
}
