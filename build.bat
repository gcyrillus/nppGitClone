@echo off
cls
REM nppGitClone Build Script (x64, /MD)
REM Compile with MSVC for Notepad++ plugin
REM Inspired by proven BuildTools setup

REM Force UTF-8 encoding
@chcp 65001 > nul

echo ==================================================
echo COMPILATION X64 : nppGitClone.dll (MSVC Native)
echo ==================================================

REM 1. ISOLEMENT : Isolate environment to avoid accumulation
setlocal

REM Work in script's own directory
cd /d "%~dp0"

REM 2. Check if MSVC environment is already active
if "%VSCMD_ARG_TGT_ARCH%"=="x64" (
    echo [INFO] MSVC x64 environment already active. Starting build...
    goto compile
)

REM 3. Try to find Visual Studio or BuildTools
REM Try VS 2026 Community first
set "VS_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2026\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2026\Community"
    echo [FOUND] Visual Studio 2026 Community
    goto found_vs
)

REM Try VS 2026 BuildTools
if exist "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
    echo [FOUND] Visual Studio 2026 BuildTools
    goto found_vs
)

REM Try VS 2022 Community
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community"
    echo [FOUND] Visual Studio 2022 Community
    goto found_vs
)

REM Try VS 2022 BuildTools
if exist "C:\Program Files (x86)\Microsoft Visual Studio\17\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\17\BuildTools"
    echo [FOUND] Visual Studio 2022 BuildTools
    goto found_vs
)

echo [ERREUR] Visual Studio ou BuildTools non trouvé.
echo Installez Visual Studio 2022/2026 Community ou BuildTools avec support C++
pause
exit /b 1

:found_vs
set "VCVARS_PATH=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"

echo Configuration de l'environnement Windows SDK et MSVC en x64...
echo.

REM Clean temporary PATH for vcvarsall
set "PATH=C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\"

REM Load MSVC x64 environment
call "%VCVARS_PATH%" amd64

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

REM Compile all source files
REM /O2 = Optimize for speed
REM /MD = Multi-threaded DLL runtime
REM /W4 = Warning level 4
REM /EHsc = C++ exception handling
REM /D_CRT_SECURE_NO_WARNINGS = Disable CRT security warnings
REM /utf-8 = UTF-8 source encoding

cl /O2 /MD /W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /utf-8 /Isrc\include /Fo.\obj\\ src\PluginDefinition.cpp src\nppGitClone.cpp src\Dialogs.cpp

if errorlevel 1 (
    echo.
    echo [ERREUR] La compilation a echoue.
    echo.
    pause
    exit /b 1
)

echo.
echo [OK] Compilation reussie.
echo.
echo [*] Linking DLL...
echo.

REM Link to create DLL
link /DLL /OUT:bin\x64\Release\nppGitClone.dll obj\*.obj user32.lib kernel32.lib shell32.lib shlwapi.lib

if errorlevel 1 (
    echo.
    echo [ERREUR] Le linking a echoue.
    echo.
    pause
    exit /b 1
)

echo.
echo ==================================================
echo [SUCCES] nppGitClone.dll generée avec succes !
echo ==================================================
echo.
echo Output: bin\x64\Release\nppGitClone.dll
echo.
echo Instructions d'installation:
echo 1. Copiez nppGitClone.dll vers:
echo    C:\Program Files\Notepad++\plugins\nppGitClone\
echo.
echo 2. Creez le dossier s'il n'existe pas:
echo    mkdir "C:\Program Files\Notepad++\plugins\nppGitClone"
echo.
echo 3. Redemarrez Notepad++
echo.
echo ==================================================
echo.

endlocal
pause
