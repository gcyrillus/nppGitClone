@echo off
cls
pause

@chcp 65001 > nul
echo ==================================================
echo COMPILATION X64 : nppGitClone.dll (Environnement Natif)
echo ==================================================

setlocal

cd /d "%~dp0"

:: Chemin vers les outils de build de Visual Studio
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
set "VCVARS_PATH=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"

if "%VSCMD_ARG_TGT_ARCH%"=="x64" (
    echo [INFO] Environnement MSVC x64 développeur déjà actif. Passage direct au build.
    goto compile
)

if not exist "%VCVARS_PATH%" (
    echo [ERREUR] Le script vcvarsall.bat est introuvable à l'adresse spécifiée.
    echo Vérifiez le chemin VS_PATH dans ce fichier .bat
    pause
    exit /b
)

echo Configuration de l'environnement Windows SDK et MSVC en x64...

set "PATH=C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\"

call "%VCVARS_PATH%" amd64

:compile
echo.
echo Compilation de nppGitClone.cpp...
cl /O2 /MT /D UNICODE /D _UNICODE /EHsc /utf-8 /LD src\nppGitClone.cpp src\PluginDefinition.cpp src\Dialogs.cpp user32.lib shlwapi.lib gdi32.lib shell32.lib /I src\include /Fe:nppGitClone.dll /link delayimp.lib

echo.
if %ERRORLEVEL% EQU 0 (
    echo [SUCCÈS] nppGitClone.dll 64-bit générée avec succès !
) else (
    echo [ERREUR] La compilation a échoué.
)
echo ==================================================

endlocal
pause