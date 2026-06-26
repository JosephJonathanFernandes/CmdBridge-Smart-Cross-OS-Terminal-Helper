@echo off
setlocal

echo [1/2] Configuring project with CMake...
cmake -B build
if %ERRORLEVEL% neq 0 (
    echo Configuration failed!
    exit /b %ERRORLEVEL%
)

echo [2/2] Building project...
cmake --build build
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo.
echo Build successful! 
echo You can run the application using: .\build\smart_terminal.exe
endlocal
