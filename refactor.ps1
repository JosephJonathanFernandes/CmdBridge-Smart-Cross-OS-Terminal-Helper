$ErrorActionPreference = "Stop"

Write-Host "Creating enterprise directory structure..."
New-Item -ItemType Directory -Force -Path "src"
New-Item -ItemType Directory -Force -Path "tests"
New-Item -ItemType Directory -Force -Path "docs"
New-Item -ItemType Directory -Force -Path "config"
New-Item -ItemType Directory -Force -Path "scripts"
New-Item -ItemType Directory -Force -Path ".github\workflows"

Write-Host "Removing build directory from Git cache..."
if (Test-Path "build") {
    git rm -r --cached build | Out-Null
}

Write-Host "Moving source files to src/..."
$cFiles = Get-ChildItem -Path . -Filter "*.c" -File
$hFiles = Get-ChildItem -Path . -Filter "*.h" -File

foreach ($file in $cFiles) {
    git mv $file.Name "src/$($file.Name)"
}
foreach ($file in $hFiles) {
    git mv $file.Name "src/$($file.Name)"
}

Write-Host "Refactoring structure setup complete!"
