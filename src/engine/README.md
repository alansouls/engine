# SSGE Engine Library

This directory contains the CMake configuration to build the SSGE Engine as a dynamically linked library (DLL on
Windows).

## Structure

- `CMakeLists.txt` - Main CMake configuration for the engine library
- `EngineAPI.h` - Header defining DLL export/import macros for Windows

## Building the Engine Library

### Option 1: Using the build scripts

- **Windows Batch**: Run `build_engine.bat` from the src directory
- **PowerShell**: Run `build_engine.ps1` from the src directory

### Option 2: Manual CMake build

```bash
# From the src directory
mkdir build/engine
cd build/engine
cmake -G "Visual Studio 17 2022" -A x64 ../../Pong/engine
cmake --build . --config Debug
```

## Output Files

After building, you'll find:

- **Debug Build**:
    - `build/engine/bin/Debug/SSGEEngine.dll` - The main engine library
    - `build/engine/lib/Debug/SSGEEngine.lib` - Import library for linking
- **Release Build**:
    - `build/engine/bin/Release/SSGEEngine.dll` - Optimized engine library
    - `build/engine/lib/Release/SSGEEngine.lib` - Import library for linking

## Using the Engine Library

### In CMake Projects

```cmake
# Add engine as subdirectory
add_subdirectory(Pong/engine)

# Link your executable against the engine
target_link_libraries(your_executable PRIVATE SSGEEngine)

# Include engine headers
target_include_directories(your_executable PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Pong/engine
)
```

### API Decoration

For classes and functions that need to be exported from the DLL, use the `SSGE_API` macro:

```cpp
#include "EngineAPI.h"

class SSGE_API MyEngineClass {
public:
    SSGE_API void myMethod();
};

SSGE_API void myGlobalFunction();
```

## Dependencies

The engine library depends on:

- **Vulkan SDK** - Graphics API
- **GLFW** - Window management and input
- **GLM** - Mathematics library
- **ImGui** - Immediate mode GUI (included in build)

## Platform Support

- **Windows**: Full DLL support with proper export/import decorations
- **Linux**: Shared library (.so) support
- **macOS**: Dynamic library (.dylib) support

## Notes

- The engine automatically exports all symbols on Windows using `WINDOWS_EXPORT_ALL_SYMBOLS`
- ImGui sources are compiled into the engine library
- The main application (Pong) now links against the engine library instead of compiling engine sources directly
