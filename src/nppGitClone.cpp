#include "PluginDefinition.h"
#include "Dialogs.h"
#include <stdlib.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
// Definition of the Notepad++ message to open a folder as a workspace
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

// THE MISSING FUNCTION: Tells Notepad++ that the plugin is indeed in Unicode
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

    // Strictly forbidden characters in a legitimate Git URL
    // that could alter the command line (especially quotes)
    const TCHAR* forbiddenChars = TEXT("\"';`<>");
    
    if (_tcspbrk(url, forbiddenChars) != NULL)
    {
        return false; // Contains a dangerous character
    }
    return true;
}

void cloneRepositoryCommand()
{
    TCHAR repoUrl[512] = TEXT("");
    
    // Calling our real dialog box with text field
    if (!showInputDialog(nppData._nppHandle, repoUrl, sizeof(repoUrl) / sizeof(TCHAR)))
    {
        return; 
    }
    
    // --- 1. SECURITY CLEANUP: Remove invisible spaces, \r and \n at the end of the URL ---
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
        MessageBox(nppData._nppHandle, TEXT("Please enter a valid URL."), 
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
    
    // Additional cleanup of repoName as a precaution
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

    // If cloning fails, alert and exit
    if (exitCode != 0)
    {
        TCHAR errorMsg[512];
        _stprintf_s(errorMsg, sizeof(errorMsg) / sizeof(TCHAR),
                    TEXT("Git cloning failed (Exit code: %d).\n\nCheck the repository URL or your internet connection."),
                    exitCode);
        MessageBox(nppData._nppHandle, errorMsg,
                   TEXT("nppGitClone - Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // --- SÉCURITÉ SUCCÈS : Exécuté uniquement si exitCode == 0 ---
    TCHAR message[512];
    _stprintf_s(message, sizeof(message) / sizeof(TCHAR), 
                TEXT("Repository cloned successfully!\n\nAdding to workspace..."), fullPath);
    MessageBox(nppData._nppHandle, message, TEXT("nppGitClone - Success"), MB_OK | MB_ICONINFORMATION);

    // --- DROP SIMULATION FOR THE LEFT PANEL ---
    // Prepare the Windows file Drop structure
    // A DROPFILES followed by the path terminated by a double \0\0
    size_t pathLen = _tcslen(fullPath);
    size_t bufferSize = sizeof(DROPFILES) + ((pathLen + 2) * sizeof(TCHAR));
    
    HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, bufferSize);
    if (hGlobal)
    {
        DROPFILES* df = (DROPFILES*)GlobalLock(hGlobal);
        if (df)
        {
            df->pFiles = sizeof(DROPFILES);
            df->fWide = TRUE; // Unicode mode (wchar_t) required
            
            // Copy path right after the structure
            TCHAR* pPath = (TCHAR*)((BYTE*)df + sizeof(DROPFILES));
            _tcscpy_s(pPath, pathLen + 1, fullPath);
            
            // The final double zero is guaranteed by the GHND flag of GlobalAlloc
            GlobalUnlock(hGlobal);
            
            // We send the Drop message directly to the main window of Notepad++
            SendMessage(nppData._nppHandle, WM_DROPFILES, (WPARAM)hGlobal, 0);
        }
    }
}