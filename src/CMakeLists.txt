cmake_minimum_required(VERSION 3.20)
project(Pong VERSION 1.0.0 LANGUAGES CXX)

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

# Define source files
set(PONG_SOURCES
        Pong/main.cpp
        Pong/game/PongGame.cpp
        Pong/game/components/Ball.cpp
        Pong/game/components/Racket.cpp)

# Add engine subdirectory
add_subdirectory(engine)

# Create executable
add_executable(pong ${PONG_SOURCES})

# Set target properties
target_compile_features(pong PRIVATE cxx_std_20)

# Include directories
target_include_directories(pong PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Pong
        ${Vulkan_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

# Link libraries
set(LINK_LIBRARIES Vulkan::Vulkan SSGEEngine)

list(APPEND LINK_LIBRARIES nethost)

# Add GLFW to link libraries based on what was found
if (glfw3_FOUND)
    list(APPEND LINK_LIBRARIES glfw)
elseif (glfw_FOUND)
    list(APPEND LINK_LIBRARIES glfw)
elseif (GLFW_FOUND)
    target_include_directories(pong PRIVATE ${GLFW_INCLUDE_DIRS})
    target_link_directories(pong PRIVATE ${GLFW_LIBRARY_DIRS})
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

target_link_libraries(pong PRIVATE ${LINK_LIBRARIES})

# Platform-specific settings
if (APPLE)
    # macOS specific settings
    target_include_directories(pong PRIVATE
            /Library/VulkanSDK/1.4.313.0/macOS/include
            /usr/local/include
            /opt/homebrew/include
    )

    target_link_directories(pong PRIVATE
            /Library/VulkanSDK/1.4.313.0/macOS/lib
            /usr/local/lib
            /opt/homebrew/lib
            ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )

    # Set rpath for runtime library loading
    set_target_properties(pong PROPERTIES
            INSTALL_RPATH "/Library/VulkanSDK/1.4.313.0/macOS/lib;/usr/local/lib;/opt/homebrew/lib;${CMAKE_BINARY_DIR}/lib"
            BUILD_WITH_INSTALL_RPATH TRUE
    )

elseif (UNIX AND NOT APPLE)
    # Linux specific settings
    target_include_directories(pong PRIVATE
            /usr/include
            /usr/local/include
    )

elseif (WIN32)
    # Windows specific settings - use your exact paths from build.zig
    target_include_directories(pong PRIVATE
            "C:/VulkanSDK/1.3.290.0/Include"
            "C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glfw-3.4.bin.WIN64/include"
            "C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glm"
    )

    target_link_directories(pong PRIVATE
            "C:/VulkanSDK/1.3.290.0/Lib"
            "C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glfw-3.4.bin.WIN64/lib-vc2022"
            "${CMAKE_CURRENT_SOURCE_DIR}/vendor/nethost/lib"
    )

    # Override library linking for Windows with your specific setup
    # Clear any previous GLFW linking attempts and use your specific libraries
    get_target_property(CURRENT_LIBRARIES pong LINK_LIBRARIES)
    if (CURRENT_LIBRARIES)
        # Remove any glfw references
        list(FILTER CURRENT_LIBRARIES EXCLUDE REGEX "glfw.*")
        set_target_properties(pong PROPERTIES LINK_LIBRARIES "${CURRENT_LIBRARIES}")
    endif ()

    # Add Windows-specific libraries
    target_link_libraries(pong PRIVATE glfw3 vulkan-1)
endif ()

# Set output directory
set_target_properties(pong PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy nethost.dll to output directory on Windows
if (WIN32)
    add_custom_command(TARGET pong POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/nethost/lib/nethost.dll"
        $<TARGET_FILE_DIR:pong>
        COMMENT "Copying nethost.dll to output directory"
    )
endif ()

# Install target
install(TARGETS pong
        RUNTIME DESTINATION bin
)
