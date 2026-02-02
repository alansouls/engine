# Pong Engine

A cross-platform C++20 game engine built with Vulkan and GLFW, featuring a Pong game implementation.

## Prerequisites

### All Platforms

- CMake 3.20 or higher
- C++20 compatible compiler
- Vulkan SDK
- GLFW
- GLM (OpenGL Mathematics)

### Windows

- Visual Studio 2019/2022 with C++ support
- vcpkg (recommended for dependency management)

### Linux

- GCC 10+ or Clang 12+
- Package manager dependencies:
  ```bash
  # Ubuntu/Debian
  sudo apt install cmake build-essential libglfw3-dev libvulkan-dev libglm-dev vulkan-tools
  
  # Fedora
  sudo dnf install cmake gcc-c++ glfw-devel vulkan-devel glm-devel vulkan-tools
  
  # Arch Linux
  sudo pacman -S cmake gcc glfw vulkan-devel glm vulkan-tools
  ```

### macOS

- Xcode with Command Line Tools
- Homebrew:
  ```bash
  brew install cmake glfw glm
  ```
- Vulkan SDK for macOS from LunarG

## Building

### Method 1: Using CMake Presets (Recommended)

Configure and build for your platform:

```bash
# Windows (Debug)
cmake --preset windows-default
cmake --build --preset windows-debug

# Windows (Release)
cmake --preset windows-release
cmake --build --preset windows-release

# Linux (Debug)
cmake --preset linux-default
cmake --build --preset linux-debug

# Linux (Release)
cmake --preset linux-release
cmake --build --preset linux-release

# macOS (Debug)
cmake --preset macos-default
cmake --build --preset macos-debug

# macOS (Release)
cmake --preset macos-release
cmake --build --preset macos-release
```

### Method 2: Manual CMake

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Install (optional)
cmake --install .
```

### Method 3: Using vcpkg (Windows)

If you have vcpkg installed:

```bash
# Configure with vcpkg toolchain
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Release
```

## Running

After building, the executable will be located in:

- `build/bin/pong` (Linux/macOS)
- `build/bin/Release/pong.exe` (Windows)

Make sure the compiled shaders (*.spv files) are in the same directory as the executable.

## Project Structure

```
src/
├── CMakeLists.txt          # Main CMake configuration
├── CMakePresets.json       # CMake presets for different platforms
├── vcpkg.json             # vcpkg dependencies
├── build.zig              # Original Zig build file (reference)
└── Pong/
    ├── main.cpp
    ├── game/              # Game-specific code
    └── engine/            # Engine code
        ├── collisions/    # Collision detection
        ├── graphics/      # Graphics rendering
        └── scenes/        # Scene management
```

## CMake vs Zig Build Comparison

The CMake configuration replicates all functionality from the original `build.zig`:

- ✅ Cross-platform support (Windows, Linux, macOS)
- ✅ C++20 standard with appropriate compiler flags
- ✅ Vulkan and GLFW linking
- ✅ Platform-specific include/library paths
- ✅ Shader file copying
- ✅ Multiple build configurations (Debug/Release)

## Troubleshooting

### Vulkan SDK Not Found

Make sure the Vulkan SDK is properly installed and the `VULKAN_SDK` environment variable is set.

### GLFW Not Found

- **Windows**: Use vcpkg or manually set paths in CMakeLists.txt
- **Linux**: Install development packages for your distribution
- **macOS**: Install via Homebrew

### Shader Files Missing

The build system automatically copies shader files from `Pong/engine/graphics/drivers/shaders/compiled/` to the output
directory. Ensure these files exist and have the `.spv` extension.

### Build Errors

1. Verify all dependencies are installed
2. Check that your compiler supports C++20
3. Ensure Vulkan SDK version matches your system
4. For Windows, verify Visual Studio version compatibility

## Command Line Options

The engine editor supports the following command line options:

### `--dotnet-project-path <path>`

Specify the path to the .NET project directory containing the C# scripts.

```bash
# Example
./pong --dotnet-project-path /path/to/your/dotnet/project
```

### `--debug-paths`

Use relative paths based on the executable location. This option automatically calculates the path to the .NET project assuming the executable is located in `src/cmake_build/bin/` (or similar build directory structure).

The relative path calculation: `executable_dir/../../dotnet/SSGEDotNet`

```bash
# Example - for development builds
./pong --debug-paths
```

This is particularly useful during development when the executable is in a build directory within the source tree.

### `--help`

Display usage information and available command line options.

```bash
./pong --help
```

### Usage Examples

```bash
# Running with debug paths (development)
cd build/bin
./pong --debug-paths

# Running with a custom .NET project path
./pong --dotnet-project-path /home/user/projects/MyDotNetProject

# Running in production with a specific path
./pong --dotnet-project-path /opt/engine/dotnet/SSGEDotNet
```
