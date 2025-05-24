const std = @import("std");

const targets: []const std.Target.Query = &.{
    .{ .cpu_arch = .aarch64, .os_tag = .macos },
    .{ .cpu_arch = .aarch64, .os_tag = .linux },
    .{ .cpu_arch = .x86_64, .os_tag = .linux, .abi = .gnu },
    .{ .cpu_arch = .x86_64, .os_tag = .windows },
};

// All C++ source files in the project
const cpp_sources = [_][]const u8{
    "Pong/main.cpp",
    "Pong/game/PongGame.cpp",
    "Pong/game/gameObjects/Ball.cpp",
    "Pong/game/gameObjects/Racket.cpp",
    "Pong/engine/collisions/CollisionManager.cpp",
    "Pong/engine/collisions/CircleCollider.cpp",
    "Pong/engine/collisions/QuadCollider.cpp",
    "Pong/engine/scenes/Scene.cpp",
    "Pong/engine/scenes/GameObject.cpp",
    "Pong/engine/scenes/Game.cpp",
    "Pong/engine/graphics/renderers/CircleItem.cpp",
    "Pong/engine/graphics/renderers/Renderer.cpp",
    "Pong/engine/graphics/renderers/RectangleItem.cpp",
    "Pong/engine/graphics/drivers/VulkanDriver.cpp",
};

pub fn build(b: *std.Build) !void {
    // Add option to specify target from command line
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // If a specific target is provided via CLI, use it; otherwise build for all targets
    if (target.query.cpu_arch != null or target.query.os_tag != null) {
        // Build for the specified target only
        const exe = b.addExecutable(.{
            .name = "pong",
            .target = target,
            .optimize = optimize,
        });

        buildExecutable(b, exe, target.query);

        const target_output = b.addInstallArtifact(exe, .{});
        b.getInstallStep().dependOn(&target_output.step);
    } else {
        // Build for all predefined targets
        for (targets) |t| {
            const exe = b.addExecutable(.{
                .name = "pong",
                .target = b.resolveTargetQuery(t),
                .optimize = optimize,
            });

            buildExecutable(b, exe, t);

            const target_output = b.addInstallArtifact(exe, .{
                .dest_dir = .{
                    .override = .{
                        .custom = try t.zigTriple(b.allocator),
                    },
                },
            });

            b.getInstallStep().dependOn(&target_output.step);
        }
    }
}

fn buildExecutable(b: *std.Build, exe: *std.Build.Step.Compile, target_query: std.Target.Query) void {
    // Platform-specific compiler flags
    var flags = std.ArrayList([]const u8).init(std.heap.page_allocator);
    defer flags.deinit();
    
    // Common flags for all platforms
    flags.appendSlice(&.{
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-DGLFW_INCLUDE_VULKAN",
        "-DGLFW_INCLUDE_NONE",
    }) catch unreachable;
    
    // Platform-specific flags
    switch (target_query.os_tag.?) {
        .macos => {
            flags.append("-DMACOSX") catch unreachable;
        },
        else => {},
    }
    
    // Add all C++ source files
    for (cpp_sources) |source| {
        exe.addCSourceFile(.{
            .file = b.path(source),
            .flags = flags.items,
        });
    }

    // Link with C++ standard library
    exe.linkLibCpp();

    // Add include directories
    exe.addIncludePath(b.path("Pong"));

    // Platform-specific library linking
    switch (target_query.os_tag.?) {
        .macos => {
            // On macOS, link with system frameworks and libraries
            exe.linkSystemLibrary("glfw");
            exe.linkSystemLibrary("vulkan");

            // Add Vulkan SDK paths for macOS
            exe.addLibraryPath(.{ .cwd_relative = "/Library/VulkanSDK/1.4.313.0/macOS/lib" });
            exe.addIncludePath(.{ .cwd_relative = "/Library/VulkanSDK/1.4.313.0/macOS/include" });

            // Add standard macOS paths
            exe.addLibraryPath(.{ .cwd_relative = "/usr/local/lib" });
            exe.addIncludePath(.{ .cwd_relative = "/usr/local/include" });

            // Add Homebrew paths as well
            exe.addLibraryPath(.{ .cwd_relative = "/opt/homebrew/lib" });
            exe.addIncludePath(.{ .cwd_relative = "/opt/homebrew/include" });

            // Add custom lib folder relative to project (if you have one)
            exe.addLibraryPath(b.path("lib"));

            // Add rpath for runtime library loading
            exe.addRPath(.{ .cwd_relative = "/Library/VulkanSDK/1.4.313.0/macOS/lib" });
            exe.addRPath(.{ .cwd_relative = "/usr/local/lib" });
            exe.addRPath(.{ .cwd_relative = "/opt/homebrew/lib" });
            exe.addRPath(b.path("lib"));

            // Link with macOS frameworks needed for GLFW and Vulkan
        },
        .linux => {
            // On Linux, link with system libraries
            exe.linkSystemLibrary("glfw");
            exe.linkSystemLibrary("vulkan");

            // Add common Linux include paths
            exe.addIncludePath(.{ .cwd_relative = "/usr/include" });
            exe.addIncludePath(.{ .cwd_relative = "/usr/local/include" });
        },
        .windows => {
            // On Windows, link with system libraries
            exe.linkSystemLibrary("glfw3");
            exe.linkSystemLibrary("vulkan-1");

            // Uncomment and adjust paths as needed for your Windows setup
            // exe.addIncludePath(.{ .cwd_relative = "C:/vcpkg/installed/x64-windows/include" });
            // exe.addLibraryPath(.{ .cwd_relative = "C:/vcpkg/installed/x64-windows/lib" });
        },
        else => {},
    }
}
