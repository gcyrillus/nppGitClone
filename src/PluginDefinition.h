#ifndef PLUGIN_DEFINITION_H
#define PLUGIN_DEFINITION_H

#include <windows.h>
#include "Notepad_plus_msgs.h"

// Plugin command IDs
#define CMD_CLONE_REPO 0

// External variables
extern HINSTANCE g_hInst;
extern NppData nppData;

// Plugin functions
void pluginInit(HANDLE hModule);
void commandMenuInit();
void commandMenuCleanUp();
void pluginCleanUp();
void cloneRepository();

#endif // PLUGIN_DEFINITION_H
