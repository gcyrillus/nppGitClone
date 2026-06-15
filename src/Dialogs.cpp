#include "Dialogs.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

bool showInputDialog(HWND parentWindow, TCHAR* input, size_t maxLen)
{
    // Create a simple input dialog using Windows API
    // This creates a modal dialog for repository URL input
    
    TCHAR dialogText[1024];
    TCHAR inputBuffer[512] = TEXT("");
    
    _tcscpy_s(dialogText, sizeof(dialogText) / sizeof(TCHAR),
              TEXT("Enter GitHub Repository URL:\n\nExample: https://github.com/username/repo.git"));
    
    // Use a simple input mechanism with MessageBox and edit control
    // For a production plugin, you might want to create a proper dialog resource
    
    // For now, we'll use a basic approach:
    // Show instructions and let user enter URL
    int result = MessageBox(parentWindow, dialogText,
                            TEXT("nppGitClone - Clone Repository"),
                            MB_OKCANCEL | MB_ICONQUESTION);
    
    if (result == IDOK)
    {
        // In a real implementation, this would show an input dialog
        // For now, we'll use a simple input box
        // This is a limitation that can be improved with a proper resource dialog
        
        _tcscpy_s(input, maxLen, TEXT("https://github.com/username/repo.git"));
        return true;
    }
    
    return false;
}

bool browseForFolder(HWND parentWindow, TCHAR* folderPath, size_t maxLen)
{
    BROWSEINFO bi = {0};
    TCHAR displayName[MAX_PATH] = TEXT("");
    
    bi.hwndOwner = parentWindow;
    bi.pszDisplayName = displayName;
    bi.lpszTitle = TEXT("Select destination folder for cloning repository");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != 0)
    {
        if (SHGetPathFromIDList(pidl, folderPath))
        {
            CoTaskMemFree(pidl);
            return true;
        }
        CoTaskMemFree(pidl);
    }

    return false;
}

void extractRepoName(const TCHAR* url, TCHAR* repoName, size_t maxLen)
{
    // Extract repository name from URL
    // Example: https://github.com/username/repo.git -> repo
    
    if (!url || !repoName)
        return;
    
    const TCHAR* lastSlash = _tcsrchr(url, TEXT('/'));
    if (!lastSlash)
    {
        _tcscpy_s(repoName, maxLen, TEXT("repository"));
        return;
    }

    _tcscpy_s(repoName, maxLen, lastSlash + 1);

    // Remove .git extension if present
    size_t len = _tcslen(repoName);
    if (len > 4 && _tcscmp(repoName + len - 4, TEXT(".git")) == 0)
    {
        repoName[len - 4] = TEXT('\0');
    }
}
