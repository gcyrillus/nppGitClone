#include "PluginDefinition.h"
#include "Dialogs.h"
#include <stdlib.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
// Définition du message Notepad++ pour ouvrir un dossier comme espace de travail
#ifndef NPPM_OPENFOLDERASWORKSPACE
    #define NPPM_OPENFOLDERASWORKSPACE (WM_USER + 1096)
#endif
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

void cloneRepositoryCommand();

extern HINSTANCE g_hInst;
extern NppData nppData;

static FuncItem funcFunc[1];

// ============================================================================
// Notepad++ Plugin Interface Functions
// ============================================================================

extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
    nppData = notepadPlusData;
    pluginInit(g_hInst);
}

extern "C" __declspec(dllexport) const TCHAR* getName()
{
    return TEXT("nppGitClone");
}

// LA FONCTION MANQUANTE : Dit à Notepad++ que le plugin est bien en Unicode
extern "C" __declspec(dllexport) bool isUnicode()
{
    return true;
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int *nbFunc)
{
    *nbFunc = 1;
    
    wcscpy_s(funcFunc[0]._itemName, 64, L"Clone Repository"); 
    funcFunc[0].pFunc = cloneRepositoryCommand;
    funcFunc[0].cmdID = CMD_CLONE_REPO;
    funcFunc[0].init2Check = false;
    funcFunc[0].pShortcutKey = NULL;
    
    return funcFunc;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode)
{
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

// ============================================================================
// Plugin Command Implementation
// ============================================================================
bool isValidGitUrl(const TCHAR* url)
{
    if (!url || _tcslen(url) == 0) return false;

    // Caractères strictement interdits dans une URL Git légitime 
    // qui pourraient altérer la ligne de commande (notamment les guillemets)
    const TCHAR* forbiddenChars = TEXT("\"';`<>");
    
    if (_tcspbrk(url, forbiddenChars) != NULL)
    {
        return false; // Contient un caractère dangereux
    }
    return true;
}

void cloneRepositoryCommand()
{
    TCHAR repoUrl[512] = TEXT("");
    
    // Appel de notre vraie boite de dialogue avec champ texte
    if (!showInputDialog(nppData._nppHandle, repoUrl, sizeof(repoUrl) / sizeof(TCHAR)))
    {
        return; 
    }
    
    // --- 1. NETTOYAGE SÉCURITÉ : On retire les espaces, \r et \n invisibles en fin d'URL ---
    size_t urlLen = _tcslen(repoUrl);
    while (urlLen > 0 && (repoUrl[urlLen - 1] == TEXT(' ') || 
                          repoUrl[urlLen - 1] == TEXT('\r') || 
                          repoUrl[urlLen - 1] == TEXT('\n')))
    {
        repoUrl[urlLen - 1] = TEXT('\0');
        urlLen--;
    }
    
    if (_tcslen(repoUrl) == 0)
    {
        MessageBox(nppData._nppHandle, TEXT("Veuillez entrer une URL valide."), 
        TEXT("nppGitClone"), MB_OK | MB_ICONWARNING);
        return;
    }
    
    TCHAR cloneDir[MAX_PATH];
    if (!browseForFolder(nppData._nppHandle, cloneDir, sizeof(cloneDir) / sizeof(TCHAR)))
    {
        return; 
    }
    
    TCHAR repoName[MAX_PATH];
    extractRepoName(repoUrl, repoName, sizeof(repoName) / sizeof(TCHAR));
    
    // Nettoyage subsidiaire du repoName par précaution
    size_t nameLen = _tcslen(repoName);
    while (nameLen > 0 && (repoName[nameLen - 1] == TEXT(' ') || 
                           repoName[nameLen - 1] == TEXT('\r') || 
                           repoName[nameLen - 1] == TEXT('\n')))
    {
        repoName[nameLen - 1] = TEXT('\0');
        nameLen--;
    }
    
    TCHAR fullPath[MAX_PATH];
    _tcscpy_s(fullPath, sizeof(fullPath) / sizeof(TCHAR), cloneDir);
    if (fullPath[_tcslen(fullPath) - 1] != TEXT('\\'))
    {
        _tcscat_s(fullPath, sizeof(fullPath) / sizeof(TCHAR), TEXT("\\"));
    }
    _tcscat_s(fullPath, sizeof(fullPath) / sizeof(TCHAR), repoName);
    
    // --- 1. URL SANITIZATION AND VALIDATION ---
    if (!isValidGitUrl(repoUrl))
    {
        MessageBox(nppData._nppHandle, 
            TEXT("The repository URL contains invalid or dangerous characters.\nOperation cancelled for security reasons."), 
            TEXT("nppGitClone - Security"), MB_OK | MB_ICONERROR);
        return;
    }

    /// --- 2. REMOVAL OF CMD /C ---
    // We call "git" directly. Quoted arguments are isolated at the system level,
    // making shell-type command injections (&, |, &&) completely harmless.
    TCHAR command[1024];
    _stprintf_s(command, sizeof(command) / sizeof(TCHAR), 
                TEXT("git clone \"%s\" \"%s\""), repoUrl, fullPath);
    
    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    PROCESS_INFORMATION pi = {0};
    
    // CreateProcess will automatically look for "git.exe" in the system PATH
    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        MessageBox(nppData._nppHandle, 
            TEXT("Failed to execute git clone.\n\nPlease ensure Git is installed and present in your system PATH environment variable."),
            TEXT("nppGitClone - Error"), MB_OK | MB_ICONERROR);
        return;
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // En cas d'erreur de clonage, on alerte et on quitte
    if (exitCode != 0)
    {
        TCHAR errorMsg[512];
        _stprintf_s(errorMsg, sizeof(errorMsg) / sizeof(TCHAR),
                    TEXT("Le clonage Git a échoué (Code de sortie: %d).\n\nVérifiez l'URL du dépôt ou votre connexion internet."),
                    exitCode);
        MessageBox(nppData._nppHandle, errorMsg,
                   TEXT("nppGitClone - Erreur"), MB_OK | MB_ICONERROR);
        return;
    }

    // --- SÉCURITÉ SUCCÈS : Exécuté uniquement si exitCode == 0 ---
    TCHAR message[512];
    _stprintf_s(message, sizeof(message) / sizeof(TCHAR), 
                TEXT("Dépôt cloné avec succès !\n\nAjout à l'espace de travail..."), fullPath);
    MessageBox(nppData._nppHandle, message, TEXT("nppGitClone - Succès"), MB_OK | MB_ICONINFORMATION);

    // --- SIMULATION DE DROP POUR LE PANNEAU DE GAUCHE ---
    // On prépare la structure de Drop de fichiers Windows
    // Un DROPFILES suivi du chemin terminé par un double \0\0
    size_t pathLen = _tcslen(fullPath);
    size_t bufferSize = sizeof(DROPFILES) + ((pathLen + 2) * sizeof(TCHAR));
    
    HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, bufferSize);
    if (hGlobal)
    {
        DROPFILES* df = (DROPFILES*)GlobalLock(hGlobal);
        if (df)
        {
            df->pFiles = sizeof(DROPFILES);
            df->fWide = TRUE; // Mode Unicode (wchar_t) indispensable
            
            // Copie du chemin juste après la structure
            TCHAR* pPath = (TCHAR*)((BYTE*)df + sizeof(DROPFILES));
            _tcscpy_s(pPath, pathLen + 1, fullPath);
            
            // Le double zéro final est garanti par le flag GHND de GlobalAlloc
            GlobalUnlock(hGlobal);
            
            // On envoie le message de Drop directement à la fenêtre principale de Notepad++
            SendMessage(nppData._nppHandle, WM_DROPFILES, (WPARAM)hGlobal, 0);
        }
    }
}