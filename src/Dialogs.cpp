#include "Dialogs.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>
#include <tchar.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

// Structure to pass data to the window
struct InputDialogData {
    TCHAR* buffer;
    size_t maxLen;
    bool confirmed;
};

// Procedure for managing the input window
LRESULT CALLBACK InputDialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static InputDialogData* data = NULL;
    static HWND hEdit = NULL;

    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT* cs = (CREATESTRUCT*)lp;
            data = (InputDialogData*)cs->lpCreateParams;

            // Instruction text
            CreateWindowEx(0, TEXT("STATIC"), TEXT("Enter the URL of the public Git repository (HTTPS) :"),
                           WS_CHILD | WS_VISIBLE,
                           15, 15, 360, 20, hwnd, NULL, NULL, NULL);

            // Input field (Edit Control)
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
                                   WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                   15, 40, 350, 24, hwnd, NULL, NULL, NULL);

            SetFocus(hEdit);

            // OK button
            CreateWindowEx(0, TEXT("BUTTON"), TEXT("OK"),
                           WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                           185, 75, 80, 25, hwnd, (HMENU)IDOK, NULL, NULL);

            // Cancel button
            CreateWindowEx(0, TEXT("BUTTON"), TEXT("Cancel"),
                           WS_CHILD | WS_VISIBLE,
                           280, 75, 80, 25, hwnd, (HMENU)IDCANCEL, NULL, NULL);

            // Apply a clean system font (Segoe UI)
            HFONT hFont = CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                                     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                                     DEFAULT_PITCH | FF_DONTCARE, TEXT("Segoe UI"));
            if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

            HWND hChild = GetWindow(hwnd, GW_CHILD);
            while (hChild) {
                SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, TRUE);
                hChild = GetWindow(hChild, GW_HWNDNEXT);
            }
            return 0;
        }
        case WM_COMMAND:
        {
            int id = LOWORD(wp);
            if (id == IDOK)
            {
                if (hEdit && data)
                {
                    GetWindowText(hEdit, data->buffer, (int)data->maxLen);
                }
                if (data) data->confirmed = true;
                DestroyWindow(hwnd);
            }
            else if (id == IDCANCEL)
            {
                if (data) data->confirmed = false;
                DestroyWindow(hwnd);
            }
            return 0;
        }
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
}

bool showInputDialog(HWND parentWindow, TCHAR* input, size_t maxLen)
{
    LPCWSTR className = TEXT("NppGitCloneInputDlg");
    WNDCLASS wc = {0};
    wc.lpfnWndProc = InputDialogProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = className;
    
    RegisterClass(&wc);

    int width = 395;
    int height = 150;
    int x = 0, y = 0;
    if (parentWindow)
    {
        RECT rect;
        GetWindowRect(parentWindow, &rect);
        x = rect.left + (rect.right - rect.left - width) / 2;
        y = rect.top + (rect.bottom - rect.top - height) / 2;
    }

    InputDialogData data = { input, maxLen, false };

    if (parentWindow) EnableWindow(parentWindow, FALSE);

    HWND hwnd = CreateWindowEx(WS_EX_DLGMODALFRAME, className, TEXT("nppGitClone - Cloner"),
                               WS_POPUP | WS_CAPTION | WS_SYSMENU,
                               x, y, width, height, parentWindow, NULL, wc.hInstance, &data);

    if (!hwnd)
    {
        if (parentWindow) EnableWindow(parentWindow, TRUE);
        UnregisterClass(className, wc.hInstance);
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Modal message loop (also handles Enter and Esc)
    MSG msg;
    while (IsWindow(hwnd) && GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_KEYDOWN)
        {
            if (msg.wParam == VK_RETURN)
            {
                SendMessage(hwnd, WM_COMMAND, IDOK, 0);
                continue;
            }
            if (msg.wParam == VK_ESCAPE)
            {
                SendMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (parentWindow)
    {
        EnableWindow(parentWindow, TRUE);
        SetFocus(parentWindow);
    }

    UnregisterClass(className, wc.hInstance);
    return data.confirmed && (_tcslen(input) > 0);
}

bool browseForFolder(HWND parentWindow, TCHAR* folderPath, size_t maxLen)
{
    BROWSEINFO bi = {0};
    TCHAR displayName[MAX_PATH] = TEXT("");
    
    bi.hwndOwner = parentWindow;
    bi.pszDisplayName = displayName;
    bi.lpszTitle = TEXT("Select the destination folder for cloning :");
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
    if (!url || !repoName)
        return;
    
    const TCHAR* lastSlash = _tcsrchr(url, TEXT('/'));
    if (!lastSlash)
    {
        _tcscpy_s(repoName, maxLen, TEXT("repository"));
        return;
    }

    _tcscpy_s(repoName, maxLen, lastSlash + 1);

    size_t len = _tcslen(repoName);
    if (len > 4 && _tcscmp(repoName + len - 4, TEXT(".git")) == 0)
    {
        repoName[len - 4] = TEXT('\0');
    }
}