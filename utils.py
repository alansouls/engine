#!/usr/bin/env python3
"""
Utility functions for the engine project.
"""

import os
import subprocess
import sys
from pathlib import Path

def compile_shaders():
    """Quick function to compile shaders using the main script."""
    script_path = Path(__file__).parent / "compile_shaders.py"
    if script_path.exists():
        subprocess.run([sys.executable, str(script_path)])
    else:
        print("compile_shaders.py not found!")

def clean_compiled_shaders():
    """Clean compiled shader files."""
    compiled_dir = Path(__file__).parent / "src/engine/graphics/drivers/shaders/compiled"
    if compiled_dir.exists():
        for spv_file in compiled_dir.glob("*.spv"):
            spv_file.unlink()
            print(f"Deleted {spv_file.name}")
        print("Cleaned compiled shaders.")
    else:
        print("Compiled shaders directory not found.")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        if sys.argv[1] == "compile":
            compile_shaders()
        elif sys.argv[1] == "clean":
            clean_compiled_shaders()
        else:
            print("Usage: python utils.py [compile|clean]")
    else:
        print("Available commands:")
        print("  python utils.py compile  - Compile all shaders")
        print("  python utils.py clean    - Clean compiled shaders")
