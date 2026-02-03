#include "SSGEEditor.h"
#include "utils/ArgumentParser.h"
#include <cstdlib>

#ifdef NDEBUG
constexpr bool debugModeOn = false;
#else
constexpr bool debugModeOn = true;
#endif

int main(int argc, char *argv[])
{
    // Parse command line arguments
    auto args = SSGE::Utils::ArgumentParser::parse(argc, argv);

    if (args.shouldExit)
    {
        return args.exitCode;
    }

    try
    {
        SSGEEditor app;
        app.run(debugModeOn, args.dotnetProjectPath);
    }
    catch (const std::exception &e)
    {
        std::printf("%s\n", e.what());
        throw;
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
