Write-Host "Building SSGE Engine Library..." -ForegroundColor Green

# Create build directory for engine
if (!(Test-Path "build\engine")) {
    New-Item -ItemType Directory -Path "build\engine" -Force
}
Set-Location "build\engine"

# Configure CMake for engine library
Write-Host "Configuring CMake..." -ForegroundColor Yellow
cmake -G "Visual Studio 17 2022" -A x64 ../../Pong/engine

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build the engine library
Write-Host "Building engine library..." -ForegroundColor Yellow
cmake --build . --config Debug

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Engine library build complete!" -ForegroundColor Green
Write-Host ""
Write-Host "DLL will be located in: build/engine/bin/Debug/SSGEEngine.dll" -ForegroundColor Cyan
Write-Host "Import library will be located in: build/engine/lib/Debug/SSGEEngine.lib" -ForegroundColor Cyan

# Return to original directory
Set-Location "../.."
