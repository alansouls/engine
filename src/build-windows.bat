@echo off
echo Building Pong Engine with CMake (Windows-specific configuration)...

REM Create build directory if it doesn't exist
if not exist "build-windows" mkdir build-windows

REM Change to build directory
cd build-windows

REM Configure with CMake using the Windows-specific CMakeLists
echo Configuring project with Windows-specific paths...
copy ..\CMakeLists-windows.txt CMakeLists.txt
cmake . -G "Visual Studio 17 2022" -A x64

REM Check if configuration was successful
if %ERRORLEVEL% neq 0 (
    echo Configuration failed!
    echo.
    echo Make sure the following paths exist on your system:
    echo - C:/VulkanSDK/1.3.290.0/
    echo - C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glfw-3.4.bin.WIN64/
    echo - C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glm/
    echo.
    pause
    exit /b %ERRORLEVEL%
)

REM Build the project
echo Building project...
cmake --build . --config Release

REM Check if build was successful
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo Build completed successfully!
echo Executable location: build-windows\bin\pong.exe

REM Return to source directory
cd ..

pause
