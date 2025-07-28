@echo off
echo Building SSGE Engine Library...

REM Create build directory for engine
if not exist "build\engine" mkdir build\engine
cd build\engine

REM Configure CMake for engine library
cmake -G "Visual Studio 17 2022" -A x64 ../../Pong/engine

REM Build the engine library
cmake --build . --config Debug

echo Engine library build complete!
echo.
echo DLL will be located in: build/engine/bin/Debug/SSGEEngine.dll
echo Import library will be located in: build/engine/lib/Debug/SSGEEngine.lib

pause
