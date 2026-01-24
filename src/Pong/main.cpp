#include "SSGEEditor.h"
#include <cstdlib>

#ifdef NDEBUG
constexpr bool debugModeOn = false;
#else
constexpr bool debugModeOn = true;
#endif

int main()
{
    try
    {
        SSGEEditor app;
        app.run(debugModeOn);
    }
    catch (const std::exception &e)
    {
        std::printf("%s\n", e.what());
        throw;
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
