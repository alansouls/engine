cmake_minimum_required(VERSION 3.20)
project(SSGEEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
if (MSVC)
    # For MSVC, use specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive-")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif ()

# Define preprocessor macros
add_compile_definitions(GLFW_INCLUDE_VULKAN GLFW_INCLUDE_NONE)

# Platform-specific definitions
if (APPLE)
    add_compile_definitions(MACOSX)
elseif (WIN32)
    add_compile_definitions(WINDOWS)
else ()
    add_compile_definitions(LINUX)
endif ()

# Find required packages
find_package(Vulkan REQUIRED)

# Try to find GLFW3 using different methods
find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    find_package(glfw QUIET)
    if (NOT glfw_FOUND)
        # Fallback: try to find GLFW using PkgConfig
        find_package(PkgConfig QUIET)
        if (PkgConfig_FOUND)
            pkg_check_modules(GLFW QUIET glfw3)
        endif ()
    endif ()
endif ()

# Try to find GLM
find_package(glm QUIET)
if (NOT glm_FOUND)
    # GLM is header-only, so we can work without find_package
    message(STATUS "GLM package not found, assuming header-only installation")
endif ()

# Define engine source files
set(ENGINE_SOURCES
        collisions/CollisionManager.cpp
        collisions/Collider.cpp
        collisions/CircleCollider.cpp
        collisions/QuadCollider.cpp
        scenes/Scene.cpp
        scenes/GameObject.cpp
        scenes/GameObjectInterop.cpp
        scenes/Game.cpp
        graphics/renderers/scene/CircleItem.cpp
        graphics/renderers/scene/RectangleItem.cpp
        graphics/renderers/scene/EditorSceneRenderer.cpp
        graphics/renderers/Renderer.cpp
        graphics/drivers/VulkanDriver.cpp
        graphics/renderers/ui/UIRenderer.cpp
        graphics/renderers/ui/views/SceneView.cpp
        graphics/EngineWindow.cpp
        graphics/renderers/scene/SceneImage.cpp
        graphics/renderers/ui/views/SceneExplorerView.cpp
        graphics/utils/Transform.cpp
        graphics/utils/TransformInterop.cpp
        scenes/components/RendererComponent.cpp
        scenes/components/QuadRendererComponent.cpp
        scenes/components/CircleRendererComponent.cpp
        scripts/components/ScriptComponent.cpp
        scripts/CSharpExecutionEngine.cpp
        input/InputManager.cpp
        input/InputState.cpp
        input/InputStateInterop.cpp
        scenes/components/ComponentType.h
        graphics/renderers/scene/SceneRenderer.cpp
        graphics/renderers/scene/SceneRenderer.h
        graphics/renderers/scene/SceneCamera.cpp
        graphics/renderers/scene/SceneCamera.h)

# Define ImGui source files that the engine depends on
set(IMGUI_SOURCES
        ../vendor/imgui/imgui.cpp
        ../vendor/imgui/imgui_draw.cpp
        ../vendor/imgui/imgui_widgets.cpp
        ../vendor/imgui/backends/imgui_impl_glfw.cpp
        ../vendor/imgui/backends/imgui_impl_vulkan.cpp
        ../vendor/imgui/imgui_demo.cpp
        ../vendor/imgui/imgui_tables.cpp)

# Create shared library (DLL on Windows)
add_library(SSGEEngine SHARED ${ENGINE_SOURCES} ${IMGUI_SOURCES})

# Set target properties
target_compile_features(SSGEEngine PRIVATE cxx_std_20)

# Define export macros for DLL
if (WIN32)
    target_compile_definitions(SSGEEngine PRIVATE SSGE_ENGINE_EXPORTS)
    target_compile_definitions(SSGEEngine INTERFACE SSGE_ENGINE_IMPORTS)
endif ()

# Include directories
target_include_directories(SSGEEngine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${Vulkan_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/nethost/include
)

# Link libraries
set(LINK_LIBRARIES Vulkan::Vulkan)

list(APPEND LINK_LIBRARIES nethost)

# Add GLFW to link libraries based on what was found
if (glfw3_FOUND)
    list(APPEND LINK_LIBRARIES glfw)
elseif (glfw_FOUND)
    list(APPEND LINK_LIBRARIES glfw)
elseif (GLFW_FOUND)
    target_include_directories(SSGEEngine PRIVATE ${GLFW_INCLUDE_DIRS})
    target_link_directories(SSGEEngine PRIVATE ${GLFW_LIBRARY_DIRS})
    list(APPEND LINK_LIBRARIES ${GLFW_LIBRARIES})
else ()
    # Manual GLFW linking for Windows
    if (WIN32)
        list(APPEND LINK_LIBRARIES glfw3)
    else ()
        list(APPEND LINK_LIBRARIES glfw)
    endif ()
endif ()

# Add GLM to link libraries if found
if (glm_FOUND)
    list(APPEND LINK_LIBRARIES glm::glm)
endif ()

target_link_libraries(SSGEEngine PRIVATE ${LINK_LIBRARIES})

# Platform-specific settings
if (APPLE)
    # macOS specific settings
    target_include_directories(SSGEEngine PRIVATE
            /Library/VulkanSDK/1.4.313.0/macOS/include
            /usr/local/include
            /opt/homebrew/include
    )

    target_link_directories(SSGEEngine PRIVATE
            /Library/VulkanSDK/1.4.313.0/macOS/lib
            /usr/local/lib
            /opt/homebrew/lib
            ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/nethost/lib
    )

    # Set rpath for runtime library loading
    set_target_properties(SSGEEngine PROPERTIES
            INSTALL_RPATH "/Library/VulkanSDK/1.4.313.0/macOS/lib;/usr/local/lib;/opt/homebrew/lib;${CMAKE_CURRENT_SOURCE_DIR}/../../lib;
            /usr/local/share/dotnet/packs/Microsoft.NETCore.App.Host.osx-arm64/9.0.0/runtimes/osx-arm64/native"
            BUILD_WITH_INSTALL_RPATH TRUE
    )

elseif (UNIX AND NOT APPLE)
    # Linux specific settings
    target_include_directories(SSGEEngine PRIVATE
            /usr/include
            /usr/local/include
    )

    target_link_directories(SSGEEngine PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/nethost/lib"
    )

elseif (WIN32)
    # Windows specific settings
    target_include_directories(SSGEEngine PRIVATE
            "C:/VulkanSDK/1.3.290.0/Include"
            "C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glfw-3.4.bin.WIN64/include"
            "C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glm"
    )

    target_link_directories(SSGEEngine PRIVATE
            "C:/VulkanSDK/1.3.290.0/Lib"
            "C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glfw-3.4.bin.WIN64/lib-vc2022"
            "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/nethost/lib"
    )

    # Override library linking for Windows with your specific setup
    get_target_property(CURRENT_LIBRARIES SSGEEngine LINK_LIBRARIES)
    if (CURRENT_LIBRARIES)
        # Remove any glfw references
        list(FILTER CURRENT_LIBRARIES EXCLUDE REGEX "glfw.*")
        set_target_properties(SSGEEngine PROPERTIES LINK_LIBRARIES "${CURRENT_LIBRARIES}")
    endif ()

    # Add Windows-specific libraries
    target_link_libraries(SSGEEngine PRIVATE glfw3 vulkan-1)
endif ()

# Set output directory
set_target_properties(SSGEEngine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Set DLL export/import decorations
if (WIN32)
    set_target_properties(SSGEEngine PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif ()

# Install target
install(TARGETS SSGEEngine
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY .
        DESTINATION include/engine
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
