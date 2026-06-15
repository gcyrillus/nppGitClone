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
    
    TCHAR command[1024];
    _stprintf_s(command, sizeof(command) / sizeof(TCHAR), 
    TEXT("cmd /c git clone \"%s\" \"%s\""), repoUrl, fullPath);
    
    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    PROCESS_INFORMATION pi = {0};
    
    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        MessageBox(nppData._nppHandle, 
            TEXT("Impossible d'exécuter git clone.\n\nVérifiez que Git est bien installé."),
        TEXT("nppGitClone - Erreur"), MB_OK | MB_ICONERROR);
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