// Notepad++ Plugin API
// This header defines the interface between Notepad++ and plugins
// Based on official Notepad++ plugin documentation

#ifndef NOTEPAD_PLUS_MSGS_H
#define NOTEPAD_PLUS_MSGS_H

#include <windows.h>

// Notepad++ messages base
#define NPPM_START (WM_USER + 0)

// Common Notepad++ messages
#define NPPM_GETNBSESSIONFILES                  (NPPM_START + 47)
#define NPPM_GETSESSIONFILES                    (NPPM_START + 48)
#define NPPM_SAVESESSION                        (NPPM_START + 53)
#define NPPM_SAVECURRENTSESSION                 (NPPM_START + 54)
#define NPPM_GETFULLCURRENTPATH                 (NPPM_START + 55)
#define NPPM_GETCURRENTSCINTILLA                (NPPM_START + 56)
#define NPPM_GETCURRENTVIEW                     (NPPM_START + 57)
#define NPPM_GETNBMAINVIEW                      (NPPM_START + 58)
#define NPPM_SETMENUITEMCHECK                   (NPPM_START + 59)
#define NPPM_GETMENUHANDLE                      (NPPM_START + 60)
#define NPPM_GETBUFFERINDEXFROMID               (NPPM_START + 61)
#define NPPM_GETIDFROMINDEX                     (NPPM_START + 62)
#define NPPM_OPEN_ALL_RECENT_FILE               (NPPM_START + 63)
#define NPPM_DELETEALLSESSION                   (NPPM_START + 64)
#define NPPM_GETCURRENTFILETYPE                 (NPPM_START + 65)
#define NPPM_GETBUFFERINDEXFROMHANDLE           (NPPM_START + 66)
#define NPPM_GETCURRENTCMDLINE                  (NPPM_START + 67)
#define NPPM_SETCURRENTCMDLINE                  (NPPM_START + 68)
#define NPPM_CHECKDOCSTATUS                     (NPPM_START + 69)
#define NPPM_GETCURRENTLANGTYPE                 (NPPM_START + 70)
#define NPPM_SETCURRENTLANGTYPE                 (NPPM_START + 71)
#define NPPM_GETBUFFERINDEXFROMFILEPATH         (NPPM_START + 72)
#define NPPM_SETBUFFERINDEXFROMFILEPATH         (NPPM_START + 73)
#define NPPM_GETRECENT_FILE_COUNT               (NPPM_START + 74)
#define NPPM_GETRECENT_FILES                    (NPPM_START + 75)
#define NPPM_GETCURRENTBUFFERID                 (NPPM_START + 76)
#define NPPM_GETCURRENTWNDHANDLE                (NPPM_START + 77)
#define NPPM_MODELESSDIALOG                     (NPPM_START + 78)
#define NPPM_GETNBDOCUMENT                      (NPPM_START + 79)
#define NPPM_GETCURRENTDOCUMENT                 (NPPM_START + 80)
#define NPPM_GETFILEBYID                        (NPPM_START + 81)
#define NPPM_GETNBSPLIT                         (NPPM_START + 82)
#define NPPM_GETDOCSPLITSTATE                   (NPPM_START + 83)
#define NPPM_ACTIVATEDOC                        (NPPM_START + 84)
#define NPPM_SAVEFILE                           (NPPM_START + 85)

// File menu commands
#define IDM_FILE_OPEN_FOLDER                    41007

// Scintilla notifications
struct SCNotification
{
    HWND hwndFrom;
    UINT idFrom;
    UINT code;
    LPARAM position;
    int ch;
    int modifiers;
    int modificationType;
    const char *text;
    int length;
    int linesAdded;
    int message;
    WPARAM wParam;
    LPARAM lParam;
    int line;
    int foldLevelNow;
    int foldLevelPrev;
};

// Notepad++ data passed to plugin
struct NppData
{
    HWND _nppHandle;
    HWND _scintillaMainHandle;
    HWND _scintillaSecondHandle;
};

// Shortcut key structure
struct ShortcutKey
{
    bool isCtrlPressed;
    bool isAltPressed;
    bool isShiftPressed;
    UCHAR key;
};

// Déclarations préalables pour le compilateur
typedef void (*PFUNCPLUGINCMD)();

struct shortcutKey {
    bool _isCtrl;
    bool _isAlt;
    bool _isShift;
    unsigned char _key;
};

// Définition propre de FuncItem
struct FuncItem {
    TCHAR _itemName[64];
    PFUNCPLUGINCMD pFunc;
    int cmdID;
    bool init2Check;
    shortcutKey *pShortcutKey;
};

#endif // NOTEPAD_PLUS_MSGS_H
