# SSGE - Super Simple Game Engine

A modern C++ game engine with C# scripting support, built with Vulkan for high-performance graphics rendering. The engine features a component-based architecture with an integrated editor for game development.

## Features

- **Vulkan-based Rendering**: High-performance graphics rendering using Vulkan API
- **C# Scripting**: Write game logic in C# with .NET integration
- **Component System**: Entity-Component architecture for flexible game object composition
- **Collision Detection**: Built-in collision system with circle and quad colliders
- **Input Management**: Cross-platform input handling
- **Scene Management**: Hierarchical scene and game object management
- **ImGui Integration**: Immediate mode GUI for editor interface
- **Cross-Platform**: Supports macOS, Windows, and Linux

## Requirements

### Build Tools
- **CMake** (3.20 or higher) - Build system generator
- **C++20 Compatible Compiler**:
  - macOS: Clang (Xcode Command Line Tools)
  - Windows: MSVC or Clang
  - Linux: GCC or Clang
- **.NET SDK** (for C# scripting support)

### Graphics & Libraries
- **Vulkan SDK** - Graphics API
- **GLFW3** - Window and input handling
- **GLM** - OpenGL Mathematics (header-only library)

### Development Tools (Recommended)
- **clangd** - Language server for C++ code intelligence
- **clang-format** - Code formatting tool

### Installation Guides

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake
brew install glfw
brew install glm
brew install llvm  # includes clangd and clang-format

# Install Vulkan SDK
# Download from: https://vulkan.lunarg.com/sdk/home
# Or use Homebrew:
brew install molten-vk vulkan-headers vulkan-loader vulkan-tools

# Install .NET SDK
brew install --cask dotnet-sdk
```

#### Windows
```powershell
# Using Chocolatey package manager
choco install cmake
choco install llvm  # includes clangd and clang-format
choco install dotnet-sdk

# Install Vulkan SDK from: https://vulkan.lunarg.com/sdk/home
# Install GLFW and GLM via vcpkg:
vcpkg install glfw3:x64-windows
vcpkg install glm:x64-windows
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake build-essential
sudo apt install libglfw3-dev
sudo apt install libglm-dev
sudo apt install clangd clang-format
sudo apt install vulkan-tools libvulkan-dev
sudo apt install dotnet-sdk-8.0
```

## Visual Studio Code Setup

### Required Extensions

Install the following extensions for the best development experience:

1. **C/C++** (`ms-vscode.cpptools`)
   - IntelliSense, debugging, and code browsing

2. **clangd** (`llvm-vs-code-extensions.vscode-clangd`)
   - Advanced C++ language server for better code intelligence
   - **Note**: Disable the C/C++ extension's IntelliSense when using clangd

3. **CMake Tools** (`ms-vscode.cmake-tools`)
   - CMake project integration and configuration

4. **CMake** (`twxs.cmake`)
   - CMake language support and syntax highlighting

5. **C#** (`ms-dotnettools.csharp`)
   - C# language support for scripting components

### Recommended Extensions

6. **CodeLLDB** (`vadimcn.vscode-lldb`)
   - Native debugger for C++ (particularly useful on macOS/Linux)

7. **Clang-Format** (`xaver.clang-format`)
   - Automatic code formatting

8. **GitLens** (`eamodio.gitlens`)
   - Enhanced Git integration

### Quick Install
```bash
# Install all required extensions at once
code --install-extension ms-vscode.cpptools
code --install-extension llvm-vs-code-extensions.vscode-clangd
code --install-extension ms-vscode.cmake-tools
code --install-extension twxs.cmake
code --install-extension ms-dotnettools.csharp
code --install-extension vadimcn.vscode-lldb
code --install-extension xaver.clang-format
```

### VS Code Configuration

Add the following to your workspace `.vscode/settings.json`:

```json
{
  "cmake.configureOnOpen": true,
  "cmake.buildDirectory": "${workspaceFolder}/cmake-build-debug",
  "C_Cpp.intelliSenseEngine": "disabled",
  "[cpp]": {
    "editor.defaultFormatter": "xaver.clang-format",
    "editor.formatOnSave": true
  },
  "[c]": {
    "editor.defaultFormatter": "xaver.clang-format",
    "editor.formatOnSave": true
  }
}
```

## Building the Project

### Using CMake (Command Line)

```bash
# Configure the project
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build cmake-build-debug

# Run the Pong demo
./cmake-build-debug/bin/pong
```

### Using VS Code

1. **Open the Project**
   - Open the `src` folder in VS Code
   - CMake Tools should automatically detect the CMakeLists.txt

2. **Configure CMake**
   - Press `Cmd+Shift+P` (macOS) or `Ctrl+Shift+P` (Windows/Linux)
   - Type "CMake: Configure" and press Enter
   - Select your compiler kit when prompted

3. **Build**
   - Press `F7` or use the build button in the status bar
   - Or use Command Palette: "CMake: Build"

4. **Run**
   - Press `Ctrl+F5` to launch without debugging
   - Or press `F5` to launch with debugging

### Build Presets

The project includes CMake presets (see `CMakePresets.json`). You can select a preset:

```bash
# List available presets
cmake --list-presets

# Configure with a preset
cmake --preset <preset-name>
```

## Project Structure

```
├── engine/              # Core engine code
│   ├── collisions/      # Collision detection system
│   ├── graphics/        # Rendering and window management
│   ├── input/           # Input handling
│   ├── scenes/          # Scene and game object management
│   └── scripts/         # C# scripting integration
├── Pong/                # Example game and editor implementation
│   ├── game/            # Pong game logic
│   └── scripts/         # C# compilation utilities
├── dotnet/              # C# .NET bindings and sample projects
│   └── SSGEDotNet/      # .NET Core API wrapper
├── vendor/              # Third-party dependencies
│   ├── imgui/           # ImGui library
│   └── nethost/         # .NET hosting libraries
└── cmake-build-debug/   # Build output directory
```

## Development Workflow

### Code Formatting

The project uses clang-format for consistent code style:

```bash
# Format a single file
clang-format -i path/to/file.cpp

# Format all C++ files
find engine Pong -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

### Debugging

1. Set breakpoints in VS Code by clicking on the left gutter
2. Press `F5` to start debugging
3. Use the Debug Console for GDB/LLDB commands

### C# Scripting

C# scripts are located in the `dotnet/SSGEDotNet` solution. To edit C# scripts:

1. Open the .sln file in Visual Studio or Rider
2. Or use VS Code with the C# extension
3. Build the .NET projects before running the engine

## Troubleshooting

### CMake can't find Vulkan
- Ensure the Vulkan SDK is installed and `VULKAN_SDK` environment variable is set
- On macOS, you may need to set: `export VULKAN_SDK=/path/to/vulkansdk`

### clangd shows errors
- Ensure `compile_commands.json` exists in the build directory
- Run CMake configure to regenerate it
- Check that clangd is pointing to the correct compile commands path

### Missing GLFW or GLM
- Install via package manager (brew, apt, vcpkg)
- Or set `CMAKE_PREFIX_PATH` to point to installation directory
- On windows set the environment variable THIRDPARTY_LIB to the root place where glm and glfw or any other future dependency will be

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Disclaimer

This is project is for studying purposes only and it is in not fit for production usage, it lacks a bunch of features a game engine should have and it's not guaranteed that it will ever be completed, feel free to use it as you like though.