# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a **Windows Shell Extension** project for adding right-click archive extraction functionality to Windows Explorer. The project targets Windows 10+ and uses C++17 with COM interfaces to integrate with the Windows shell.

**Core Functionality**: Extract compressed archives (gz, bz2, tar, tar.gz, tar.bz2) via right-click context menu.

## Development Environment Setup

### Prerequisites
```powershell
# Install required tools
choco install visualstudio2022community cmake git vcpkg

# Setup vcpkg (run from vcpkg install directory)
.\vcpkg.exe integrate install
```

### VS Code Setup
1. Install required extensions:
   - C/C++ (Microsoft) 
   - CMake Tools
   - C++ TestMate
   - Git Extension Pack

2. Create VS Code workspace configuration:
```json
// .vscode/settings.json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.compilerPath": "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.xx.xxxxx\\bin\\Hostx64\\x64\\cl.exe",
    "C_Cpp.default.windowsSdkVersion": "10.0.22000.0",
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}\\build"
}
```

## Build System

### Initial Project Setup
```powershell
# Configure vcpkg dependencies
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake"

# Build the project  
cmake --build build --config Debug

# For release builds
cmake --build build --config Release
```

### Key Dependencies (vcpkg.json)
- **zlib** - gzip compression/decompression
- **bzip2** - bzip2 compression/decompression  
- **gtest** - unit testing framework
- Optional: **libtar** or **microtar** for tar archive handling

### VS Code Build Tasks
```json
// .vscode/tasks.json - Configure build tasks
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Configure",
            "type": "shell",
            "command": "cmake",
            "args": ["-B", "build", "-S", ".", "-G", "Visual Studio 17 2022"]
        },
        {
            "label": "CMake Build", 
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build", "--config", "Debug"],
            "group": {"kind": "build", "isDefault": true},
            "dependsOn": "CMake Configure"
        }
    ]
}
```

## COM Shell Extension Architecture

### Core COM Interfaces
The shell extension implements two primary COM interfaces:

```cpp
class ArchiveExtractor : public IContextMenu, public IShellExtInit {
public:
    // IShellExtInit - Initialize with selected files
    HRESULT Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID);
    
    // IContextMenu - Handle context menu operations  
    HRESULT QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    HRESULT InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
    HRESULT GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, LPSTR pszName, UINT cchMax);
};
```

### Project Structure
```
src/
├── shell-extension/     # COM implementation, registry handling
├── extraction-engine/   # Compression library integration (zlib, bzip2, libtar)  
├── ui-components/       # Progress dialogs, error handling
└── utilities/          # Common helpers, file I/O, path handling

tests/                  # Google Test unit and integration tests
resources/              # Icons, string resources, registry templates
installer/              # MSI installer configuration
```

## Development Workflow

### Daily Development
```powershell
# Build and test cycle
cmake --build build --config Debug
cd build && ctest --output-on-failure

# Register shell extension for testing (requires Admin)
regsvr32 "build\bin\Debug\ArchiveExtractorShell.dll"

# Unregister after testing
regsvr32 /u "build\bin\Debug\ArchiveExtractorShell.dll"
```

### Debugging Shell Extensions
1. Build debug version of DLL
2. Register DLL with `regsvr32`
3. In VS Code: Debug → Attach to Process → select `explorer.exe`
4. Set breakpoints in shell extension code
5. Right-click on archive files to trigger breakpoints

### Testing
```powershell
# Run all tests
cd build && ctest

# Run specific test categories  
cd build && ctest -R "GzipExtractor.*"
cd build && ctest -R "ShellExtension.*"

# Run with verbose output
cd build && ctest --verbose
```

## Windows-Specific Considerations

### COM Registration
- **Development**: Use `regsvr32` for manual registration
- **Production**: Registry scripts in `resources/` directory
- **Cleanup**: Always unregister before uninstalling

### File Path Handling
- Support paths >260 characters using `\\?\` prefix
- Handle Unicode filenames correctly
- Validate extracted paths to prevent directory traversal

### Security Best Practices
- Archive bomb protection (size limits, extraction timeouts)
- Run in user context (no elevation required)
- Validate all file paths during extraction
- Handle corrupted archive files gracefully

### Performance Requirements
- Context menu display: <100ms
- Progress updates: Every 1% or 10MB
- Memory usage during extraction: <100MB
- Stream-based processing for large files

## Testing Strategy

### Test Categories
1. **Unit Tests**: Individual compression libraries, file I/O utilities
2. **COM Interface Tests**: Shell extension initialization, context menu handling  
3. **Integration Tests**: End-to-end extraction workflows
4. **Performance Tests**: Large file handling, memory usage validation

### Test Data Location
```
tests/test-archives/
├── small/           # <1MB test files
├── medium/          # 50-100MB test files  
├── large/           # >500MB test files
├── unicode/         # International character filenames
└── corrupted/       # Malformed archives for error testing
```

## Troubleshooting

### Common Issues

**COM Registration Fails**:
- Ensure running as Administrator
- Check DLL dependencies with `depends.exe`
- Verify correct architecture (x64 vs x86)

**Context Menu Not Appearing**:
- Restart Windows Explorer (`taskkill /f /im explorer.exe && explorer.exe`)
- Check Windows Registry entries for shell extension
- Verify file association registry keys

**Build Failures**:
- Ensure vcpkg is properly integrated
- Update Windows SDK if COM headers are missing
- Check compiler architecture matches vcpkg triplet

**Debug Breakpoints Not Hit**:
- Verify DLL is registered and loaded by Explorer
- Use Process Monitor to confirm DLL loading
- Check that debug symbols are generated

### Development Notes

- Always test shell extensions in clean Windows VMs before release
- Use RAII for COM object lifetime management  
- Handle Windows Explorer crashes gracefully
- Test with various archive sizes and file system types (NTFS, exFAT)

## Key Documentation Files

- `project-plan.md` - Complete technical architecture and implementation phases
- `project-memory.md` - Technical decisions, rationale, and architectural choices  
- `test-cases.md` - Comprehensive testing strategy and test case specifications