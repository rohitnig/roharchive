# Windows Archive Extractor

A lightweight Windows Shell Extension that adds right-click context menu functionality for extracting compressed archives (gz, bz2, tar, tar.gz, tar.bz2).

## Development Status

**Current Phase**: Phase 1 - Development Environment Setup ✅

### Phase 1 Completed:
- [x] VS Code workspace configuration
- [x] vcpkg dependency management setup  
- [x] CMake build system configuration
- [x] Basic project structure creation
- [x] Git repository initialization

### Next Steps:
- Phase 2: COM Foundation implementation
- Phase 3: Core Extraction Engine
- Phase 4: Shell Integration
- Phase 5: User Interface
- Phase 6: Testing & Polish

## Prerequisites

- Windows 10/11
- Visual Studio 2022 Community (with C++ development tools)
- CMake 3.20+
- vcpkg
- Git

## Quick Start

1. Clone the repository
2. Install vcpkg dependencies:
   ```powershell
   vcpkg install
   ```
3. Configure the project:
   ```powershell
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```
4. Build:
   ```powershell
   cmake --build build --config Debug
   ```

## Project Structure

```
src/
├── shell-extension/     # COM Shell Extension implementation
├── extraction-engine/   # Core compression/decompression logic  
├── ui-components/       # Progress dialogs and user interface
└── utilities/          # Helper functions and common code

tests/                  # Unit and integration tests
resources/              # Icons, strings, registry templates
.vscode/               # VS Code configuration
```

For detailed documentation, see:
- `project-plan.md` - Complete technical architecture and implementation phases
- `project-memory.md` - Technical decisions and architectural choices  
- `test-cases.md` - Testing strategy and test case specifications
- `CLAUDE.md` - Development guidelines and setup instructions