
# nppGitClone

A lightweight Notepad++ plugin that allows you to clone GitHub repositories directly from the editor interface and seamlessly add them to your workspace without cluttering your tabs.

## Features

- **Intuitive UI:** Simple input dialog to paste your public GitHub repository URL.
- **Background Execution:** Automated, non-blocking `git clone` process.
- **Workspace Integration:** Directly adds the cloned repository to the "Folder as Workspace" side panel via a simulated drag-and-drop mechanism.
- **Smart Path Handling:** Configurable destination directory via a native Windows folder browser.
- **Multi-Architecture Support:** Ready-to-use scripts for compiling in both Windows x64 and x86 environments using MSVC.

## Installation

### Manual Installation
1. Download the latest release: [nppGitClone.zip](https://github.com/gcyrillus/nppGitClone/releases/download/V1.0.0/nppGitClone.zip)
2. Unzip the archive to a temporary folder.
3. Navigate to your Notepad++ plugins directory (usually `C:\Program Files\Notepad++\plugins\`).
4. Create a new folder named exactly `nppGitClone`.
5. Copy all files from the temporary folder into this new `plugins\nppGitClone\` directory.
6. Restart Notepad++ to activate the plugin.

## Build Instructions

### Prerequisites

- **Visual Studio 2026** (with C++ Desktop Development build tools)
- **Git for Windows** (with `git.exe` added to your system PATH)
- **Windows SDK**

### Building with Batch Files

Open your terminal or double-click the corresponding script depending on your target Notepad++ architecture:

* For a **64-bit (x64)** build, run:

```bash
  build.bat
```

* For a **32-bit (x86)** build, run:

```bash
buildx86.bat
```



The compilation automation scripts will:

* Safely initialize the temporary MSVC developer environment.
* Compile all internal source files using the appropriate multi-threaded library flag (`/MD`).
* Link against essential Windows API libraries (`user32`, `shell32`, `shlwapi`, `gdi32`).
* Generate a standalone `nppGitClone.dll` compiled for your targeted architecture.

## Usage

1. Launch Notepad++.
2. Navigate to the top menu: `Plugins` > `nppGitClone` > `Clone Repository`.
3. Enter the public GitHub HTTPS URL (e.g., `https://github.com/username/repo.git`).
4. Select your preferred local destination folder using the browser window.
5. Relax! The plugin handles the cloning and appends the root folder to your left workspace layout automatically.

## Project Structure

```text
nppGitClone/
├── src/
│   ├── PluginDefinition.h       # Plugin interface definitions
│   ├── PluginDefinition.cpp     # Plugin initialization & registration
│   ├── nppGitClone.cpp          # Main operational logic
│   ├── Dialogs.h                # Dialog UI windows declarations
│   ├── Dialogs.cpp              # Custom input and directory dialogue implementations
│   └── include/
│       ├── Notepad_plus_msgs.h  # Standard Notepad++ messaging API
│       ├── Scintilla.h          # Scintilla components integration keys
│       └── resource.h           # Win32 resource identifiers
├── build.bat                    # Native 64-bit automation script
├── buildx86.bat                 # Cross-compilation 32-bit automation script
├── README.md                    # Product manual (This file)
└── .gitignore                   # Workspace untracked file signatures

```

## Troubleshooting

### Build fails: "Could not find Visual Studio"

* Ensure that Visual Studio 2026 Build Tools are correctly deployed.
* Open `build.bat` or `buildx86.bat` and verify that the `VS_PATH` parameter reflects your actual IDE installation directory.
* Try running your terminal instance or the batch files as **Administrator**.

### Plugin doesn't appear in Notepad++

* Confirm the target installation folder layout matches: `...\Notepad++\plugins\nppGitClone\nppGitClone.dll`.
* Check your exact Notepad++ architecture architecture via *Help > About Notepad++*. Loading an x86 DLL inside an x64 app layer will cause an silent initialization fault. Ensure you executed the correct compilation batch script.
* Always restart the host editor process completely after copying new assets.

### Git clone fails

* Ensure Git is globally accessible. Try opening a standalone Command Prompt and type `git --version` to test your environment path.
* Check active network configurations and ensure the destination disk has write permissions.

## License

This project is licensed under the MIT License.

## Author

**gcyrillus**

