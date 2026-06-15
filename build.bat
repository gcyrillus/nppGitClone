@echo off
cls
setlocal enabledelayedexpansion

echo ==================================================
echo COMPILATION X64 : nppGitClone.dll (MSVC Native)
echo ==================================================

cd /d "%~dp0"

if "%VSCMD_ARG_TGT_ARCH%"=="x64" (
    echo [INFO] MSVC x64 environment already active. Starting build...
    goto compile
)

set "VS_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2026\Community"
    echo [FOUND] Visual Studio 2026 Community
    goto found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
    echo [FOUND] Visual Studio 2026 BuildTools
    goto found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community"
    echo [FOUND] Visual Studio 2022 Community
    goto found_vs
)

echo [ERREUR] Visual Studio ou BuildTools non trouve.
pause
exit /b 1

:found_vs
set "VCVARS_PATH=!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat"

echo Configuration de l'environnement MSVC en x64...
set "PATH=C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\"

call "!VCVARS_PATH!" amd64

:compile
echo.
echo [*] Creating output directories...
if not exist "bin\x64\Release" mkdir bin\x64\Release
if not exist "obj" mkdir obj

echo [*] Cleaning previous build...
if exist "obj\*.obj" del obj\*.obj >nul 2>&1
if exist "bin\x64\Release\nppGitClone.dll" del bin\x64\Release\nppGitClone.dll >nul 2>&1

echo.
echo [*] Compiling source files...
echo.

cl /O2 /MD /W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /Isrc\include /Fo.\obj\\ src\PluginDefinition.cpp src\nppGitClone.cpp src\Dialogs.cpp

if errorlevel 1 (
    echo.
    echo [ERREUR] Compilation echouee.
    echo.
    pause
    exit /b 1
)

echo.
echo [OK] Compilation reussie.
echo.
echo [*] Linking DLL...
echo.

link /DLL /OUT:bin\x64\Release\nppGitClone.dll obj\*.obj user32.lib kernel32.lib shell32.lib shlwapi.lib

if errorlevel 1 (
    echo.
    echo [ERREUR] Linking echoue.
    echo.
    pause
    exit /b 1
)

echo.
echo ==================================================
echo [SUCCES] nppGitClone.dll generee !
echo ==================================================
echo.
echo Output: bin\x64\Release\nppGitClone.dll
echo.
endlocal
pause
