# nppGitClone

A Notepad++ plugin that allows you to clone GitHub repositories directly from Notepad++ and open them as a workspace.

## Features

- Input dialog for GitHub repository URL
- Automatic `git clone` execution
- Opens cloned repository as Notepad++ workspace
- Configurable clone destination directory
- Built for Windows x64 with MSVC

## Installation

1. Build the plugin (see Build Instructions below)
2. Copy the compiled DLL to: `C:\Program Files\Notepad++\plugins\nppGitClone\`
3. Restart Notepad++

## Build Instructions

### Prerequisites

- Visual Studio 2022 or later (with C++ compiler)
- Git for Windows installed (for `git.exe`)
- Windows SDK

### Building with batch file

```bash
build.bat
```

The batch file will:
- Set up the MSVC compiler environment (x64)
- Compile all source files with `/MD` flag
- Link with required Windows libraries
- Generate `nppGitClone.dll` in the `bin\x64\Release` directory

### Manual compilation

If you prefer to compile manually:

```bash
REM Set up MSVC compiler
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

Rem Create output directory
mkdir obj
mkdir bin\x64\Release

REM Compile all source files
cl /MD /O2 /W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /Isrc\include /Fo.\obj\\ src\PluginDefinition.cpp src\nppGitClone.cpp src\Dialogs.cpp

REM Link to create DLL
link /DLL /OUT:bin\x64\Release\nppGitClone.dll obj\*.obj user32.lib kernel32.lib shell32.lib
```

## Usage

1. Open Notepad++
2. Go to `Plugins` > `nppGitClone` > `Clone Repository`
3. Enter the GitHub repository URL (e.g., `https://github.com/username/repo.git`)
4. Select the destination folder where you want the repository to be cloned
5. The plugin will clone the repository and open it as a workspace

## Project Structure

```
nppGitClone/
├── src/
│   ├── PluginDefinition.h       # Plugin interface definitions
│   ├── PluginDefinition.cpp     # Plugin initialization
│   ├── nppGitClone.cpp          # Main plugin logic
│   ├── Dialogs.h                # Dialog function declarations
│   ├── Dialogs.cpp              # Dialog implementations
│   └── include/
│       ├── Notepad_plus_msgs.h  # Notepad++ API
│       ├── Scintilla.h          # Scintilla editor messages
│       └── resource.h           # Resource IDs
├── bin/                         # Build output directory (generated)
├── obj/                         # Object files directory (generated)
├── build.bat                    # Build script
├── README.md                    # This file
└── .gitignore                   # Git ignore rules
```

## Troubleshooting

### Build fails: "Could not find Visual Studio"
- Make sure Visual Studio 2022 is installed
- Check that the path in `build.bat` matches your installation
- Try running Developer Command Prompt as Administrator

### Plugin doesn't appear in Notepad++
- Verify the DLL is in the correct folder: `C:\Program Files\Notepad++\plugins\nppGitClone\`
- Make sure it's named exactly `nppGitClone.dll`
- Restart Notepad++ after copying the DLL
- Check that Notepad++ is 64-bit (this plugin is x64 only)

### Git clone fails
- Ensure Git for Windows is installed and accessible from command line
- Test: Open Command Prompt and type `git --version`
- Make sure you have internet access
- Verify the GitHub URL is correct

## License

MIT License

## Author

gcyrillus
