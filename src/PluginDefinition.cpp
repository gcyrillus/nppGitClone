#include "PluginDefinition.h"
#include <tchar.h>

// Global variables
HINSTANCE g_hInst = NULL;
NppData nppData;

void pluginInit(HANDLE hModule)
{
    g_hInst = (HINSTANCE)hModule;
}

void commandMenuInit()
{
    // Menu items will be initialized in getFuncsArray
}

void commandMenuCleanUp()
{
    // Cleanup if needed
}

void pluginCleanUp()
{
    // Final cleanup
}

void cloneRepository()
{
    // This is called from the main plugin file
}
