#!/usr/bin/env python3
"""
Script to prepare the project for debugging.
This script:
1. Configures and compiles the CMake C++ project
2. Moves nethost dynamic library to the appropriate platform-specific location
"""

import os
import sys
import shutil
import platform
import subprocess
from pathlib import Path


def get_platform():
    """Detect the current platform."""
    system = platform.system()
    if system == "Windows":
        return "windows"
    elif system == "Linux":
        return "linux"
    elif system == "Darwin":
        return "macos"
    else:
        raise RuntimeError(f"Unsupported platform: {system}")


def run_command(cmd, cwd=None, description=None):
    """Run a shell command and handle errors."""
    if description:
        print(f"\n{'='*60}")
        print(f"⚙️  {description}")
        print(f"{'='*60}")
    
    print(f"Running: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            check=True,
            shell=(platform.system() == "Windows"),
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True
        )
        if result.stdout:
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Error: Command failed with exit code {e.returncode}")
        if e.stdout:
            print(e.stdout)
        return False

def configure_cmake(src_dir, build_dir, preset="vscode-debug"):
    """Configure CMake project."""
    success = run_command(
        ["cmake", "--preset", preset],
        cwd=src_dir,
        description="Configuring CMake project"
    )
    
    if not success:
        print("❌ CMake configuration failed")
        return False
    
    return True


def build_cmake(build_dir):
    """Build CMake project."""
    success = run_command(
        ["cmake", "--build", str(build_dir)],
        cwd=build_dir,
        description="Building CMake project"
    )
    
    if not success:
        print("❌ CMake build failed")
        return False
    
    return True

def copy_nethost_library(vendor_dir, cpp_build_dir, current_platform):
    """Copy nethost dynamic library to appropriate location based on platform."""
    print(f"\n{'#'*60}")
    print("# Copying Nethost Library")
    print(f"{'#'*60}")
    
    nethost_lib_dir = vendor_dir / "nethost" / "lib"
    
    if not nethost_lib_dir.exists():
        print(f"❌ Nethost library directory not found: {nethost_lib_dir}")
        return False
    
    # Determine source and destination based on platform
    if current_platform == "windows":
        src_file = nethost_lib_dir / "nethost.dll"
        dest_dir = cpp_build_dir / "bin"
    elif current_platform == "linux":
        src_file = nethost_lib_dir / "libnethost.so"
        dest_dir = cpp_build_dir / "bin"
    elif current_platform == "macos":
        src_file = nethost_lib_dir / "libnethost.dylib"
        dest_dir = cpp_build_dir / "lib"
    else:
        print(f"❌ Unknown platform: {current_platform}")
        return False
    
    if not src_file.exists():
        print(f"❌ Nethost library not found: {src_file}")
        return False
    
    if not dest_dir.exists():
        print(f"Creating directory: {dest_dir}")
        dest_dir.mkdir(parents=True, exist_ok=True)
    
    dest_file = dest_dir / src_file.name
    
    try:
        shutil.copy2(src_file, dest_file)
        print(f"✓ Copied {src_file.name} to {dest_dir}")
        return True
    except Exception as e:
        print(f"❌ Failed to copy nethost library: {e}")
        return False


def main():
    """Main function to orchestrate the build process."""
    print(f"\n{'═'*60}")
    print("  🚀 Project Debug Preparation Script")
    print(f"{'═'*60}\n")
    
    # Detect platform
    current_platform = get_platform()
    print(f"📍 Detected platform: {current_platform}")
    
    # Get project directories
    script_dir = Path(__file__).parent.resolve()
    src_dir = script_dir / "src"
    vendor_dir = src_dir / "vendor"
    build_dir = src_dir / "cmake-build-debug"
    cpp_bin_dir = build_dir / "bin"
    
    # Verify directories exist
    if not src_dir.exists():
        print(f"❌ Source directory not found: {src_dir}")
        sys.exit(1)
    
    # Configure CMake
    if not configure_cmake(src_dir, build_dir):
        print("\n❌ CMake configuration failed")
        sys.exit(1)
    
    # Build CMake project
    if not build_cmake(build_dir):
        print("\n❌ CMake build failed")
        sys.exit(1)
    
    # Copy nethost library
    if not copy_nethost_library(vendor_dir, build_dir, current_platform):
        print("\n❌ Failed to copy nethost library")
        sys.exit(1)
    
    # Success!
    print(f"\n{'═'*60}")
    print("  ✅ Project successfully prepared for debugging!")
    print(f"{'═'*60}")
    print(f"\n📂 Executable location: {cpp_bin_dir / 'pong'}")
    print(f"🔧 Build directory: {build_dir}\n")


if __name__ == "__main__":
    main()
