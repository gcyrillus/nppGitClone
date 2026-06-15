#ifndef DIALOGS_H
#define DIALOGS_H

#include <windows.h>

// Dialog functions
bool showInputDialog(HWND parentWindow, TCHAR* input, size_t maxLen);
bool browseForFolder(HWND parentWindow, TCHAR* folderPath, size_t maxLen);
void extractRepoName(const TCHAR* url, TCHAR* repoName, size_t maxLen);

#endif // DIALOGS_H
