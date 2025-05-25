#!/usr/bin/env python3
"""
Shader Compilation Script
Compiles GLSL shaders to SPIR-V and generates a C++ header file with embedded shader data.
"""

import os
import subprocess
import sys
from pathlib import Path
from typing import List, Tuple

# Configuration
SHADER_DIR = "src/Pong/engine/graphics/drivers/shaders"
OUTPUT_DIR = "src/Pong/engine/graphics/drivers/shaders/compiled"
HEADER_FILE = "src/Pong/engine/graphics/drivers/shaders/shaders.h"
GLSLC_COMMAND = "glslc"  # Assumes glslc is in PATH, part of Vulkan SDK

# Shader file extensions to process
SHADER_EXTENSIONS = {
    ".vert": "vertex",
    ".frag": "fragment", 
    ".geom": "geometry",
    ".comp": "compute",
    ".tesc": "tessellation_control",
    ".tese": "tessellation_evaluation"
}

class ShaderCompiler:
    def __init__(self, shader_dir: str, output_dir: str, header_file: str):
        self.shader_dir = Path(shader_dir)
        self.output_dir = Path(output_dir)
        self.header_file = Path(header_file)
        self.compiled_shaders: List[Tuple[str, str, Path]] = []
        
        # Create output directory if it doesn't exist
        self.output_dir.mkdir(parents=True, exist_ok=True)
    
    def find_shaders(self) -> List[Path]:
        """Find all shader files in the shader directory."""
        shader_files = []
        for ext in SHADER_EXTENSIONS.keys():
            shader_files.extend(self.shader_dir.glob(f"*{ext}"))
        return sorted(shader_files)
    
    def compile_shader(self, shader_path: Path) -> bool:
        """Compile a single shader file to SPIR-V."""
        # Generate output filename
        output_name = shader_path.stem + shader_path.suffix + ".spv"
        output_path = self.output_dir / output_name
        
        # Compile command
        cmd = [GLSLC_COMMAND, str(shader_path), "-o", str(output_path)]
        
        try:
            print(f"Compiling {shader_path.name}...")
            result = subprocess.run(cmd, capture_output=True, text=True, check=True)
            
            # Store info about compiled shader
            variable_name = self.generate_variable_name(shader_path)
            shader_type = SHADER_EXTENSIONS.get(shader_path.suffix, "unknown")
            self.compiled_shaders.append((variable_name, shader_type, output_path))
            
            print(f"  ✓ Success: {output_name}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"  ✗ Error compiling {shader_path.name}:")
            print(f"    {e.stderr}")
            return False
        except FileNotFoundError:
            print(f"  ✗ Error: {GLSLC_COMMAND} not found. Make sure Vulkan SDK is installed and in PATH.")
            return False
    
    def generate_variable_name(self, shader_path: Path) -> str:
        """Generate a valid C++ variable name from shader filename."""
        # Remove extension and convert to valid C++ identifier
        name = shader_path.stem
        # Replace invalid characters with underscores
        name = "".join(c if c.isalnum() else "_" for c in name)
        # Add suffix based on shader type
        suffix = shader_path.suffix[1:]  # Remove the dot
        return f"{name}_{suffix}_spv"
    
    def read_binary_file(self, file_path: Path) -> List[int]:
        """Read binary file and return as list of bytes."""
        with open(file_path, "rb") as f:
            return list(f.read())
    
    def generate_header_file(self) -> bool:
        """Generate C++ header file with embedded shader data."""
        if not self.compiled_shaders:
            print("No compiled shaders to include in header file.")
            return False
        
        try:
            with open(self.header_file, "w") as f:
                # Write header guard and includes
                f.write("#pragma once\n")
                f.write("#include <cstdint>\n")
                f.write("#include <cstddef>\n")
                f.write("#include <string>\n\n")
                f.write("// Auto-generated shader data\n")
                f.write("// DO NOT EDIT MANUALLY\n\n")
                f.write("namespace Shaders {\n\n")
                
                # Write shader data arrays
                for var_name, shader_type, spv_path in self.compiled_shaders:
                    if not spv_path.exists():
                        print(f"Warning: {spv_path} not found, skipping...")
                        continue
                    
                    # Read binary data
                    binary_data = self.read_binary_file(spv_path)
                    
                    # Write array declaration
                    f.write(f"    // {shader_type} shader: {spv_path.name}\n")
                    f.write(f"    static const uint8_t {var_name}[] = {{\n")
                    
                    # Write binary data in chunks of 12 bytes per line
                    for i in range(0, len(binary_data), 12):
                        chunk = binary_data[i:i+12]
                        hex_values = [f"0x{b:02x}" for b in chunk]
                        f.write(f"        {', '.join(hex_values)}")
                        if i + 12 < len(binary_data):
                            f.write(",")
                        f.write("\n")
                    
                    f.write(f"    }};\n")
                    f.write(f"    static const size_t {var_name}_size = sizeof({var_name});\n\n")
                
                # Write shader info structure
                f.write("    struct ShaderInfo {\n")
                f.write("        const char* name;\n")
                f.write("        const char* type;\n")
                f.write("        const uint8_t* data;\n")
                f.write("        size_t size;\n")
                f.write("    };\n\n")
                
                # Write shader registry
                f.write("    static const ShaderInfo shaders[] = {\n")
                for var_name, shader_type, spv_path in self.compiled_shaders:
                    if spv_path.exists():
                        f.write(f'        {{"{spv_path.stem}", "{shader_type}", {var_name}, {var_name}_size}},\n')
                f.write("    };\n\n")
                
                f.write(f"    static const size_t shader_count = {len([s for s in self.compiled_shaders if s[2].exists()])};\n\n")
                
                # Write convenience functions
                f.write("    // Convenience functions\n")
                f.write("    inline const ShaderInfo* findShader(const std::string& name) {\n")
                f.write("        for (size_t i = 0; i < shader_count; ++i) {\n")
                f.write("            if (shaders[i].name == name) {\n")
                f.write("                return &shaders[i];\n")
                f.write("            }\n")
                f.write("        }\n")
                f.write("        return nullptr;\n")
                f.write("    }\n\n")
                
                f.write("    inline const ShaderInfo* findShaderByType(const std::string& name, const std::string& type) {\n")
                f.write("        for (size_t i = 0; i < shader_count; ++i) {\n")
                f.write("            if (shaders[i].name == name && shaders[i].type == type) {\n")
                f.write("                return &shaders[i];\n")
                f.write("            }\n")
                f.write("        }\n")
                f.write("        return nullptr;\n")
                f.write("    }\n\n")
                
                f.write("} // namespace Shaders\n")
            
            print(f"✓ Generated header file: {self.header_file}")
            return True
            
        except Exception as e:
            print(f"✗ Error generating header file: {e}")
            return False
    
    def compile_all(self) -> bool:
        """Compile all shaders and generate header file."""
        print("=== Shader Compilation Script ===")
        print(f"Shader directory: {self.shader_dir}")
        print(f"Output directory: {self.output_dir}")
        print(f"Header file: {self.header_file}")
        print()
        
        # Find all shader files
        shader_files = self.find_shaders()
        if not shader_files:
            print("No shader files found.")
            return False
        
        print(f"Found {len(shader_files)} shader file(s):")
        for shader in shader_files:
            print(f"  - {shader.name}")
        print()
        
        # Compile each shader
        success_count = 0
        for shader_path in shader_files:
            if self.compile_shader(shader_path):
                success_count += 1
        
        print()
        print(f"Compilation complete: {success_count}/{len(shader_files)} shaders compiled successfully.")
        
        if success_count > 0:
            print()
            self.generate_header_file()
        
        return success_count == len(shader_files)

def main():
    """Main entry point."""
    # Get the script directory to resolve relative paths
    script_dir = Path(__file__).parent
    
    # Resolve paths relative to script location
    shader_dir = script_dir / SHADER_DIR
    output_dir = script_dir / OUTPUT_DIR
    header_file = script_dir / HEADER_FILE
    
    # Check if shader directory exists
    if not shader_dir.exists():
        print(f"Error: Shader directory not found: {shader_dir}")
        print("Please run this script from the engine root directory.")
        return 1
    
    # Create and run compiler
    compiler = ShaderCompiler(str(shader_dir), str(output_dir), str(header_file))
    success = compiler.compile_all()
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
