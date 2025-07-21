# Windows Archive Extractor - Project Plan

## Project Overview

**Goal**: Create a lightweight Windows Shell Extension that adds right-click context menu functionality for extracting compressed archives (bz2, gz, tar) using C++ APIs.

**Target Platform**: Windows 10+ (x64)
**Primary Language**: C++17
**Development Environment**: Visual Studio Code
**Architecture**: COM-based Shell Extension

## Core Requirements

### Supported Formats (Phase 1)
- **gzip (.gz)** - Single file compression
- **bzip2 (.bz2)** - Single file compression  
- **tar (.tar)** - Archive format (uncompressed)
- **Compound formats** - tar.gz, tar.bz2

### Key Features
- Right-click context menu integration
- Multiple file selection support
- Progress indication during extraction
- Error handling and user feedback
- Unicode filename support
- Extract to current folder or subfolder options

## Development Environment Setup

### Visual Studio Code Configuration
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

### Required VS Code Extensions
- **C/C++** (Microsoft) - IntelliSense, debugging, code browsing
- **C++ TestMate** - Test discovery and execution
- **CMake Tools** - CMake integration and build management
- **Git Extension Pack** - Git integration and version control
- **Error Lens** - Inline error highlighting
- **Bracket Pair Colorizer** - Code readability
- **Better Comments** - Enhanced comment highlighting

### Build System Integration
```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Configure",
            "type": "shell",
            "command": "cmake",
            "args": ["-B", "build", "-S", ".", "-G", "Visual Studio 17 2022"],
            "group": "build",
            "problemMatcher": ["$msCompile"]
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

## Technical Architecture

### Component Structure
```
WindowsArchiveExtractor/
├── .vscode/                # VS Code configuration
│   ├── settings.json
│   ├── tasks.json
│   ├── launch.json
│   └── c_cpp_properties.json
├── src/
│   ├── shell-extension/     # COM Shell Extension implementation
│   ├── extraction-engine/   # Core compression/decompression logic
│   ├── ui-components/       # Progress dialogs and user interface
│   └── utilities/          # Helper functions and common code
├── libs/                   # Third-party libraries
├── resources/              # Icons, strings, registry templates
├── installer/              # Installation package files
├── tests/                  # Unit and integration tests
├── CMakeLists.txt          # Main CMake configuration
└── vcpkg.json             # Package dependencies
```

### Core Libraries

#### Compression Libraries
1. **zlib** (Version 1.3+)
   - Purpose: gzip compression/decompression
   - License: zlib License (compatible)
   - Size: ~100KB compiled
   - Integration: vcpkg package

2. **bzip2** (Version 1.0.8+)
   - Purpose: bzip2 compression/decompression
   - License: BSD-style (compatible)
   - Size: ~50KB compiled
   - Integration: vcpkg package

3. **libtar** or **microtar**
   - Purpose: tar archive handling
   - License: BSD/MIT (compatible)
   - Size: ~20KB compiled
   - Alternative: Custom lightweight tar implementation

#### Windows APIs
- **Windows Shell API** - IContextMenu, IShellExtInit interfaces
- **COM APIs** - Component registration and lifecycle
- **Windows Registry API** - File association registration
- **Windows UI APIs** - Progress dialogs and error messages

### Shell Extension Implementation

#### COM Interfaces
```cpp
class ArchiveExtractor : public IContextMenu, public IShellExtInit {
public:
    // IShellExtInit
    HRESULT Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pDataObj, HKEY hKeyProgID);
    
    // IContextMenu
    HRESULT QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    HRESULT InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
    HRESULT GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, LPSTR pszName, UINT cchMax);
};
```

## Implementation Phases

### Phase 1: Development Environment Setup (Week 1) ✅ COMPLETED
- [x] VS Code workspace configuration
- [x] vcpkg dependency management setup
- [x] CMake build system configuration
- [x] Basic project structure creation
- [x] Git repository initialization

### Phase 2: COM Foundation (Week 1-2) ✅ COMPLETED
- [x] Basic COM Shell Extension skeleton
- [x] Registry registration framework
- [x] VS Code debugging configuration for DLL
- [x] Test harness for shell extension loading

### Phase 3: Core Extraction Engine (Week 2-3) ✅ COMPLETED (TAR Support)
- [x] Implement tar archive parsing
- [x] File extraction utilities with progress callbacks
- [x] Connect extraction engine to shell extension
- [x] Add extraction testing framework
- [ ] Integrate zlib for gzip support (deferred - vcpkg issues)
- [ ] Integrate bzip2 for bz2 support (deferred - vcpkg issues)
- [ ] Unit tests with VS Code Test Explorer integration (pending)

### Phase 4: Shell Integration (Week 3-4)
- [ ] Context menu implementation
- [ ] File selection handling
- [ ] Multi-file extraction support
- [ ] Integration testing framework

### Phase 5: User Interface (Week 4-5)
- [ ] Progress dialog implementation
- [ ] Error message dialogs
- [ ] Extraction options (location, overwrite)
- [ ] User settings persistence

### Phase 6: Testing & Polish (Week 5-6)
- [ ] Comprehensive testing suite
- [ ] VS Code debugging workflows
- [ ] Performance profiling integration
- [ ] Memory leak detection with VS Code tools
- [ ] Installer creation

## Build System

### CMake Configuration
```cmake
cmake_minimum_required(VERSION 3.20)
project(WindowsArchiveExtractor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg integration
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

# Main shell extension DLL
add_library(ArchiveExtractorShell SHARED ${SHELL_SOURCES})
target_link_libraries(ArchiveExtractorShell PRIVATE 
    ZLIB::ZLIB 
    BZip2::BZip2
    comctl32
    ole32
    oleaut32
    uuid
)

# Set output directory for easy testing
set_target_properties(ArchiveExtractorShell PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
```

### vcpkg Dependencies
```json
{
    "name": "windows-archive-extractor",
    "version": "1.0.0",
    "dependencies": [
        "zlib",
        "bzip2",
        {
            "name": "gtest",
            "features": ["main"]
        }
    ],
    "builtin-baseline": "latest"
}
```

## VS Code Debugging Setup

### Debug Configuration
```json
// .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Shell Extension",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "C:\\Windows\\explorer.exe",
            "args": ["C:\\temp"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "console": "externalTerminal",
            "preLaunchTask": "CMake Build"
        },
        {
            "name": "Debug Tests",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}\\build\\tests\\Debug\\archive_extractor_tests.exe",
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "console": "internalConsole",
            "preLaunchTask": "CMake Build"
        }
    ]
}
```

## Development Workflow

### Daily Development Cycle
1. **Code in VS Code** with IntelliSense support
2. **Build with Ctrl+Shift+P** → "CMake: Build"
3. **Test with integrated Test Explorer**
4. **Debug shell extension** by attaching to explorer.exe
5. **Commit changes** using built-in Git integration

### Testing Strategy
```cpp
// Example unit test structure
TEST(GzipExtractor, ExtractSingleFile) {
    GzipExtractor extractor;
    std::string input_file = "test.txt.gz";
    std::string output_dir = "temp/";
    
    EXPECT_TRUE(extractor.Extract(input_file, output_dir));
    EXPECT_TRUE(std::filesystem::exists("temp/test.txt"));
}
```

## Performance Considerations

### Development Performance
- **IntelliSense optimization** for large codebases
- **Incremental builds** with CMake
- **Parallel compilation** configuration
- **Fast debugging** with optimized symbol loading

### Runtime Performance
- **Stream-based processing** for large files
- **Chunked extraction** to limit memory footprint
- **Progress callbacks** without blocking UI
- **Efficient file I/O** with proper buffering

## Development Tools

### Required Software
- **Visual Studio Code** (latest version)
- **Microsoft C++ Build Tools** or **Visual Studio 2022 Community**
- **Windows 10/11 SDK** (latest version)
- **CMake 3.20+**
- **vcpkg** for dependency management
- **Git** for version control

### Optional Development Tools
- **Windows Subsystem for Linux** (for cross-platform testing)
- **Docker Desktop** (for isolated build environments)
- **Chocolatey** (for easy tool installation)
- **PowerShell 7+** (enhanced terminal experience)

### VS Code Workspace Benefits
- **Integrated terminal** for all command-line operations
- **IntelliSense** for COM interfaces and Windows APIs
- **Git integration** for version control
- **Extension ecosystem** for enhanced productivity
- **Cross-platform compatibility** for team development
- **Customizable build tasks** and debugging workflows

## Installation Strategy

### Installer Requirements
- **Administrative privileges** for COM registration
- **Registry modification** for shell extension registration
- **File copying** to Program Files directory
- **Uninstaller** creation

### Development Installation
```powershell
# Register DLL for development testing
regsvr32 "C:\path\to\build\bin\ArchiveExtractorShell.dll"

# Unregister for cleanup
regsvr32 /u "C:\path\to\build\bin\ArchiveExtractorShell.dll"
```

---

**Document Version**: 1.1  
**Last Updated**: July 21, 2025  
**Development Environment**: Visual Studio Code  
**Next Review**: Weekly during development phase