#include "SSGEEditor.h"
#include "nfd.h"
#include "utils/ArgumentParser.h"
#include <cstdlib>

#ifdef NDEBUG
constexpr bool debugModeOn = false;
#else
constexpr bool debugModeOn = true;
#endif

auto initDependencies() -> void
{
    NFD_Init();
}

auto cleanupDependencies() -> void
{
    NFD_Quit();
}

int main(int argc, char *argv[])
{
    initDependencies();
    // Parse command line arguments
    auto args = SSGE::Utils::ArgumentParser::parse(argc, argv);

    if (args.shouldExit)
    {
        return args.exitCode;
    }

    try
    {
        SSGE::Editor::SSGEEditor app;
        app.run(debugModeOn, args.dotnetProjectPath);
    }
    catch (const std::exception &e)
    {
        std::printf("%s\n", e.what());
        cleanupDependencies();
        throw;
    }

    cleanupDependencies();

    return EXIT_SUCCESS;
}