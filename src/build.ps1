# PowerShell build script for Pong Engine
Write-Host "Building Pong Engine with CMake..." -ForegroundColor Green

# Create build directory if it doesn't exist
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build"
    Write-Host "Created build directory" -ForegroundColor Yellow
}

try {
    # Configure with CMake
    Write-Host "Configuring project..." -ForegroundColor Cyan
    
    # Try the simple approach first (without find_package)
    if (Test-Path "CMakeLists-simple.txt") {
        Write-Host "Using simplified CMakeLists for Windows..." -ForegroundColor Yellow
        Copy-Item "CMakeLists-simple.txt" "build\CMakeLists.txt" -Force
        & cmake -B build -S build -G "Visual Studio 17 2022" -A x64
    } else {
        Write-Host "Using main CMakeLists.txt..." -ForegroundColor Yellow
        & cmake -B build -S . -G "Visual Studio 17 2022" -A x64
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Configuration failed!" -ForegroundColor Red
        Write-Host ""
        Write-Host "Make sure the following paths exist on your system:" -ForegroundColor Yellow
        Write-Host "- C:/VulkanSDK/1.3.290.0/" -ForegroundColor White
        Write-Host "- C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glfw-3.4.bin.WIN64/" -ForegroundColor White
        Write-Host "- C:/Users/maiaa/Documents/Visual Studio 2022/Libraries/glm/" -ForegroundColor White
        Write-Host ""
        Read-Host "Press Enter to continue"
        exit $LASTEXITCODE
    }
    
    # Build the project
    Write-Host "Building project..." -ForegroundColor Cyan
    & cmake --build build --config Release
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed!" -ForegroundColor Red
        Read-Host "Press Enter to continue"
        exit $LASTEXITCODE
    }
    
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "Executable location: build\bin\pong.exe" -ForegroundColor White
    
} finally {
    # Return to source directory
    Set-Location ".."
}

Read-Host "Press Enter to continue"
