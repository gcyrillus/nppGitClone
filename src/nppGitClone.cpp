#include "PluginDefinition.h"
#include "Dialogs.h"
#include <stdlib.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

// Function prototypes
void cloneRepositoryCommand();

// Forward declarations
extern HINSTANCE g_hInst;
extern NppData nppData;

// ============================================================================
// Notepad++ Plugin Interface Functions
// ============================================================================

// Called when plugin is loaded
extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
    nppData = notepadPlusData;
    pluginInit(g_hInst);
}

// Return plugin name
extern "C" __declspec(dllexport) const TCHAR* getName()
{
    return TEXT("nppGitClone");
}

// Return functions array
extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* pnbFuncItems)
{
    static FuncItem funcItem[1];

    // Command 0: Clone Repository
    funcItem[0].itemName = TEXT("Clone Repository");
    funcItem[0].pFunc = cloneRepositoryCommand;
    funcItem[0].cmdID = CMD_CLONE_REPO;
    funcItem[0].init2Check = false;
    funcItem[0].pShortcutKey = NULL;

    *pnbFuncItems = 1;
    return funcItem;
}

// Called when plugin is unloaded
extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode)
{
    // Handle notifications if needed
}

// Export message function
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
    return TRUE;
}

// ============================================================================
// Plugin Command Implementation
// ============================================================================

void cloneRepositoryCommand()
{
    // Step 1: Get repository URL from user
    TCHAR repoUrl[512] = TEXT("");
    
    if (!showInputDialog(nppData._nppHandle, repoUrl, sizeof(repoUrl) / sizeof(TCHAR)))
    {
        return;  // User cancelled
    }

    // Validate URL
    if (_tcslen(repoUrl) == 0)
    {
        MessageBox(nppData._nppHandle, TEXT("Please enter a valid GitHub URL."), 
                   TEXT("nppGitClone"), MB_OK | MB_ICONWARNING);
        return;
    }

    // Step 2: Get clone destination directory
    TCHAR cloneDir[MAX_PATH];
    if (!browseForFolder(nppData._nppHandle, cloneDir, sizeof(cloneDir) / sizeof(TCHAR)))
    {
        return;  // User cancelled folder selection
    }

    // Step 3: Extract repository name from URL
    TCHAR repoName[MAX_PATH];
    extractRepoName(repoUrl, repoName, sizeof(repoName) / sizeof(TCHAR));

    // Step 4: Build full path for cloned repository
    TCHAR fullPath[MAX_PATH];
    _tcscpy_s(fullPath, sizeof(fullPath) / sizeof(TCHAR), cloneDir);
    if (fullPath[_tcslen(fullPath) - 1] != TEXT('\\'))
    {
        _tcscat_s(fullPath, sizeof(fullPath) / sizeof(TCHAR), TEXT("\\"));
    }
    _tcscat_s(fullPath, sizeof(fullPath) / sizeof(TCHAR), repoName);

    // Step 5: Build git clone command
    TCHAR command[1024];
    _stprintf_s(command, sizeof(command) / sizeof(TCHAR), 
                TEXT("cmd /c git clone \"%s\" \"%s\""), repoUrl, fullPath);

    // Step 6: Execute git clone
    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {0};

    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        MessageBox(nppData._nppHandle, 
                   TEXT("Failed to execute git clone.\n\nMake sure Git for Windows is installed and accessible from command line."),
                   TEXT("nppGitClone - Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Step 7: Wait for git clone to complete
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (exitCode != 0)
    {
        TCHAR errorMsg[512];
        _stprintf_s(errorMsg, sizeof(errorMsg) / sizeof(TCHAR),
                    TEXT("Git clone failed (exit code: %d).\n\nPlease check:\n- Repository URL is correct\n- You have internet access\n- Git is properly installed"),
                    exitCode);
        MessageBox(nppData._nppHandle, errorMsg,
                   TEXT("nppGitClone - Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Step 8: Success message
    TCHAR message[512];
    _stprintf_s(message, sizeof(message) / sizeof(TCHAR), 
                TEXT("Repository cloned successfully!\n\nPath: %s"), fullPath);
    MessageBox(nppData._nppHandle, message, TEXT("nppGitClone - Success"), MB_OK | MB_ICONINFORMATION);
}
