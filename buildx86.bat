@echo off
cls

@chcp 65001 > nul
echo ==================================================
echo COMPILATION X86 : nppGitClone.dll (Architecture 32-bit)
echo ==================================================

setlocal

cd /d "%~dp0"

:: Chemin vers les outils de build de Visual Studio
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools"
set "VCVARS_PATH=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"

if "%VSCMD_ARG_TGT_ARCH%"=="x86" (
    echo [INFO] Environnement MSVC x86 développeur déjà actif. Passage direct au build.
    goto compile
)

if not exist "%VCVARS_PATH%" (
    echo [ERREUR] Le script vcvarsall.bat est introuvable à l'adresse spécifiée.
    echo Vérifiez le chemin VS_PATH dans ce fichier .bat
    pause
    exit /b
)

echo Configuration de l'environnement Windows SDK et MSVC en x86 (32-bit)...

set "PATH=C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\"

:: Appel de l'environnement pour cibler l'architecture x86 (32 bits)
call "%VCVARS_PATH%" x86

:compile
echo.
echo Compilation de nppGitClone.cpp en mode x86...
cl /O2 /MD /D UNICODE /D _UNICODE /EHsc /utf-8 /LD src\nppGitClone.cpp src\PluginDefinition.cpp src\Dialogs.cpp user32.lib shlwapi.lib gdi32.lib shell32.lib /I src\include /Fe:nppGitClone.dll /link delayimp.lib

echo.
if %ERRORLEVEL% EQU 0 (
    echo [SUCCÈS] nppGitClone.dll 32-bit x86 générée avec succès !
) else (
    echo [ERREUR] La compilation a échoué.
)
echo ==================================================

endlocal
pause