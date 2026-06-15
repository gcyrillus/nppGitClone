@echo off
REM nppGitClone Build Script (x64, /MD)
REM Compile with MSVC for Notepad++ plugin
REM Author: gcyrillus

setlocal enabledelayedexpansion

echo.
echo ============================================
echo nppGitClone - Build Script (x64)
echo ============================================
echo.

REM Try to set up MSVC compiler environment
REM First try Visual Studio 2026
echo [*] Setting up MSVC compiler environment...

if exist "C:\Program Files\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvars64.bat"
    goto :compiler_ok
)

REM Try Visual Studio 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    goto :compiler_ok
)

REM Try Professional edition
if exist "C:\Program Files\Microsoft Visual Studio\2026\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2026\Professional\VC\Auxiliary\Build\vcvars64.bat"
    goto :compiler_ok
)

REM Try Enterprise edition
if exist "C:\Program Files\Microsoft Visual Studio\2026\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2026\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    goto :compiler_ok
)

echo [ERROR] Could not find Visual Studio compiler.
echo Please ensure Visual Studio 2022 or 2026 is installed with C++ support.
echo.
pause
exit /b 1

:compiler_ok
echo [OK] Compiler environment ready.
echo.

REM Create output directories
echo [*] Creating output directories...
if not exist "bin\x64\Release" mkdir bin\x64\Release
if not exist "obj" mkdir obj
echo [OK] Output directories created.
echo.

REM Clean old object files
echo [*] Cleaning previous build...
if exist "obj\*.obj" del obj\*.obj >nul 2>&1
if exist "bin\x64\Release\nppGitClone.dll" del bin\x64\Release\nppGitClone.dll >nul 2>&1
echo [OK] Clean complete.
echo.

echo [*] Compiling source files...
echo.

REM Compile all source files
REM /MD = Multi-threaded DLL runtime
REM /O2 = Optimize for speed
REM /W4 = Warning level 4
REM /EHsc = C++ exception handling
cl /MD /O2 /W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /Isrc\include /Fo.\obj\\ src\PluginDefinition.cpp src\nppGitClone.cpp src\Dialogs.cpp

if errorlevel 1 (
    echo.
    echo [ERROR] Compilation failed.
    echo.
    pause
    exit /b 1
)

echo.
echo [OK] Compilation successful.
echo.
echo [*] Linking DLL...
echo.

REM Link to create DLL
link /DLL /OUT:bin\x64\Release\nppGitClone.dll obj\*.obj user32.lib kernel32.lib shell32.lib shlwapi.lib

if errorlevel 1 (
    echo.
    echo [ERROR] Linking failed.
    echo.
    pause
    exit /b 1
)

echo.
echo [OK] Linking successful.
echo.
echo ============================================
echo Build Successful!
echo ============================================
echo.
echo Output: bin\x64\Release\nppGitClone.dll
echo.
echo Installation Instructions:
echo 1. Copy nppGitClone.dll to:
echo    C:\Program Files\Notepad++\plugins\nppGitClone\
echo.
echo 2. Create the folder if it doesn't exist:
echo    mkdir "C:\Program Files\Notepad++\plugins\nppGitClone"
echo.
echo 3. Restart Notepad++
echo.
echo ============================================
echo.
pause
