# Windows Archive Extractor - Project Memory

## Project Context

**Project Name**: Windows Archive Extractor  
**Initiated**: July 21, 2025  
**Developer**: Rohit  
**Development Environment**: Visual Studio Code on Windows 10+  
**Repository**: `C:\Users\rohit\Documents\Sources\roharchive`

## Core Project Vision

Create a **lightweight, efficient Windows Shell Extension** that seamlessly integrates into Windows Explorer's right-click context menu, enabling users to extract compressed archives (bz2, gz, tar) with minimal system footprint and maximum reliability.

### Key Design Principles
1. **Lightweight**: Minimal memory usage and fast startup
2. **Native Integration**: Seamless Windows Explorer integration
3. **User-Friendly**: Simple right-click workflow
4. **Reliable**: Robust error handling and progress feedback
5. **Extensible**: Architecture supports future format additions

## Technical Decision Record

### Architecture Decisions

#### Decision: COM-based Shell Extension
**Date**: July 21, 2025  
**Context**: Need to integrate with Windows Explorer right-click context menu  
**Decision**: Implement using Windows COM (Component Object Model) Shell Extension interfaces  
**Rationale**: 
- Native Windows integration mechanism
- Supported across all Windows versions (10+)
- Minimal performance overhead
- Standard approach used by WinRAR, 7-Zip, etc.
**Alternatives Considered**: Registry-only context menu (limited functionality), standalone application (poor UX)

#### Decision: Visual Studio Code as Primary IDE
**Date**: July 21, 2025  
**Context**: Need efficient C++ development environment  
**Decision**: Use VS Code with C++, CMake, and debugging extensions  
**Rationale**:
- Lightweight and fast startup
- Excellent IntelliSense support for Windows APIs
- Integrated CMake support
- Superior Git integration
- Cross-platform team development potential
**Alternatives Considered**: Visual Studio 2022 (heavier), CLion (licensing cost)

#### Decision: Static Library Linking
**Date**: July 21, 2025  
**Context**: Minimize deployment complexity and dependencies  
**Decision**: Statically link all compression libraries (zlib, bzip2, libtar)  
**Rationale**:
- Single DLL deployment
- No version conflicts with system libraries
- Predictable behavior across Windows versions
- Easier installation and uninstallation
**Trade-offs**: Larger DLL size (~200KB vs ~50KB), but acceptable for functionality gained

#### Decision: vcpkg for Dependency Management
**Date**: July 21, 2025  
**Context**: Need reliable C++ package management  
**Decision**: Use vcpkg with CMake integration  
**Rationale**:
- Microsoft-supported package manager
- Excellent Windows compatibility
- CMake integration
- Consistent build environments
**Alternatives Considered**: Conan (complexity), manual library management (maintenance overhead)

### Library Selection Rationale

#### zlib for Gzip Support
**Why Chosen**:
- Industry standard implementation (RFC 1952)
- Mature, stable, well-tested
- Small footprint (~50KB compiled)
- Compatible zlib license
- Excellent performance characteristics

#### bzip2 for Bzip2 Support
**Why Chosen**:
- Reference implementation by Julian Seward
- Superior compression ratios vs gzip
- BSD-style license compatibility
- Minimal dependencies (~30KB compiled)
- Wide compatibility

#### libtar vs microtar Decision
**Current Status**: To be decided during implementation
**libtar Pros**: Full-featured, handles all tar variants
**microtar Pros**: Minimal size (~5KB), easier to embed
**Decision Criteria**: Feature completeness vs binary size impact

### Interface Design Decisions

#### Progress Reporting Strategy
**Decision**: Callback-based progress with separate UI thread  
**Rationale**:
- Non-blocking extraction operations
- Responsive user interface
- Cancellation support
- Memory efficient (no polling)

#### Error Handling Philosophy
**Decision**: Graceful degradation with user-friendly messages  
**Approach**:
- Continue processing other files if one fails
- Clear, actionable error messages
- Logging for debugging (optional)
- No system instability from errors

## Development Environment Configuration

### System Tool Locations
- **Git Binary**: `C:\Program Files\Git\cmd\git.exe`
- **vcpkg Installation**: `C:\Users\rohit\scoop\apps\vcpkg\current\` (via Scoop)
- **vcpkg Toolchain**: `C:\Users\rohit\scoop\apps\vcpkg\current\scripts\buildsystems\vcpkg.cmake`
- **Visual Studio 2022**: `C:\Program Files\Microsoft Visual Studio\2022\Community\`
- **MSVC Compiler**: `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64\cl.exe`
- **Windows SDK**: Version 10.0.22621.0
- **CMake**: Installed via Scoop at `C:\Users\rohit\scoop\apps\cmake\current\`

### Git Configuration
- **User**: rohitnig
- **Email**: rohitnigam@gmail.com
- **SSH Keys Location**: `C:\Users\rohit\.ssh`
- **Remote Repository**: `git@github.com:rohitnig/roharchive.git`

### vcpkg Configuration Details
- **Baseline Commit**: `ce613c41372b23b1f51333815feb3edd87ef8a8b`
- **Dependencies**: zlib, bzip2, gtest
- **Integration**: Visual Studio MSBuild integration completed

### VS Code Extensions Stack
```json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "matepek.vscode-catch2-test-adapter",
        "ms-vscode.vscode-json",
        "eamodio.gitlens",
        "gruntfuggly.todo-tree"
    ]
}
```

### Build Environment Decisions
- **Compiler**: MSVC 2022 (best Windows API compatibility)
- **C++ Standard**: C++17 (modern features, stable support)
- **CMake Version**: 3.20+ (modern CMake practices)
- **Target Architecture**: x64 (primary), x86 (future consideration)
- **Build Generator**: Visual Studio 17 2022 (for native Windows development)

### Testing Strategy Rationale
**Framework**: Google Test (gtest)  
**Why**: 
- Industry standard C++ testing
- Excellent VS Code integration
- Rich assertion library
- Mocking capabilities for COM interfaces

## Implementation Strategy

### Phase-based Development Approach
**Rationale**: Iterative development with early validation points

#### Phase 1 Priority: Proof of Concept
- Basic COM shell extension registration
- Single format extraction (gzip)
- Manual testing workflow

#### Phase 2 Priority: Core Functionality
- All three formats (gz, bz2, tar)
- Basic progress indication
- Error handling framework

#### Phase 3 Priority: Production Ready
- Comprehensive testing
- Installation package
- Performance optimization

### Risk Mitigation Strategies

#### COM Registration Conflicts
**Risk**: Shell extension conflicts with existing tools  
**Mitigation**: Unique CLSID generation, proper cleanup, conditional menu display

#### Performance Impact
**Risk**: Slow context menu or extraction performance  
**Mitigation**: Async operations, progress feedback, resource limiting, benchmarking

#### Windows Compatibility
**Risk**: Different behavior across Windows versions  
**Mitigation**: Testing on multiple Windows versions, Windows SDK best practices

## Future Roadmap Considerations

### Phase 2 Enhancements (Post-MVP)
1. **Additional Formats**: 7z, zip, xz support
2. **Advanced UI**: Extraction options dialog, settings
3. **Performance**: Multi-threaded extraction
4. **Integration**: PowerShell cmdlets, automation support

### Extensibility Architecture
**Plugin System**: Consider COM-based plugin architecture for additional formats  
**Settings System**: Registry-based user preferences  
**Localization**: Resource-based string management for i18n

## Technical Constraints and Assumptions

### Windows Version Support
**Minimum**: Windows 10 (1903+)  
**Primary Target**: Windows 11  
**Rationale**: Modern APIs, security features, development efficiency

### File Size Limitations
**Target**: Handle files up to 4GB efficiently  
**Memory Usage**: Stream-based processing to limit RAM usage to <100MB  
**Progress**: Updates every 1% or 10MB, whichever is smaller

### Security Considerations
**Path Traversal**: Validate all extracted paths (no ../ sequences)  
**Archive Bombs**: Size limits and extraction timeouts  
**Permissions**: Run in user context, no elevation required

## Lessons Learned & Best Practices

### Windows Development
- Always test COM registration in clean environments
- Use Resource Acquisition Is Initialization (RAII) for COM objects
- Handle Windows path length limitations (>260 characters)
- Test with various file system types (NTFS, exFAT)
- **IMPORTANT**: Always use Windows commands in development (ren, copy, del, dir) instead of Unix commands (mv, cp, rm, ls)
- **Git Usage**: Use full path `"C:\Program Files\Git\cmd\git.exe"` when calling Git from scripts
- **PowerShell Commands**: Use `powershell -Command` for Windows-specific operations (Remove-Item, Rename-Item, etc.)

### C++ Best Practices for This Project
- Use smart pointers for resource management
- Prefer standard library containers over raw arrays
- Exception handling for library integration
- Const-correctness for API interfaces

### VS Code Development Workflow
- Configure IntelliSense paths for Windows SDK
- Use integrated terminal for CMake operations
- Leverage debugging with DLL attachment
- Utilize task automation for repetitive builds

## Knowledge Base

### Useful Resources
- **COM Programming**: Microsoft's COM documentation, ATL framework
- **Shell Extensions**: Windows Shell programming guide, existing open-source examples
- **Compression Libraries**: Format specifications (RFC 1951, 1952), library documentation
- **Testing**: Google Test documentation, VS Code testing integration

### Code Examples and Patterns
```cpp
// COM interface implementation pattern
class ArchiveExtractor : 
    public IContextMenu,
    public IShellExtInit,
    public IUnknown
{
private:
    ULONG m_ref_count;
    std::vector<std::wstring> m_selected_files;
    
public:
    ArchiveExtractor() : m_ref_count(1) {}
    
    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
};
```

### Performance Benchmarks (Target)
- **Context Menu Display**: <100ms
- **Small File Extraction** (<1MB): <1 second
- **Large File Extraction** (100MB): <30 seconds with progress
- **Memory Usage**: <50MB during extraction
- **DLL Load Time**: <50ms

## Project Milestones

### Milestone 1: Development Environment (Week 1) ✅ COMPLETED
- [x] VS Code workspace configured
- [x] vcpkg dependencies resolved  
- [x] CMake build working
- [x] Basic project structure created
- [x] Git repository initialized with proper commits

### Milestone 2: Proof of Concept (Week 2)
- [ ] Shell extension registers successfully
- [ ] Context menu appears for .gz files
- [ ] Single file extraction working

### Milestone 3: Core Features (Week 3-4)
- [ ] All formats (gz, bz2, tar) supported
- [ ] Multi-file selection working
- [ ] Progress dialog functional

### Milestone 4: Production Ready (Week 5-6)
- [ ] Comprehensive testing complete
- [ ] Installer package created
- [ ] Documentation finalized

## Decision Log Template
```markdown
### Decision: [Title]
**Date**: [YYYY-MM-DD]
**Context**: [Why this decision was needed]
**Decision**: [What was decided]
**Rationale**: [Why this decision was made]
**Alternatives Considered**: [What else was considered]
**Trade-offs**: [What we gained/lost]
**Impact**: [How this affects the project]
```

## Phase 1 Completion Summary (July 21, 2025)

### Environment Setup Verification
- **CMake Configuration**: Working with Visual Studio 17 2022 generator
- **Build System**: Generates proper MSBuild files, compiles successfully
- **vcpkg Integration**: Configured but dependencies will be installed in Phase 2
- **VS Code Integration**: Full IntelliSense, debugging, and task automation
- **Git Repository**: Local repo with 2 clean commits, ready for GitHub push

### Key Commands for Development
```powershell
# Configure project
cmake -B build -S . -G "Visual Studio 17 2022"

# Build project
cmake --build build --config Debug

# Git operations (using full path)
"C:\Program Files\Git\cmd\git.exe" -C "C:\Users\rohit\Documents\Sources\roharchive" status

# vcpkg install (when ready)
vcpkg install
```

## Contact and Collaboration

### Development Notes for Future Team Members
- Project follows Windows development best practices
- All major decisions documented in this file
- Test-driven development approach
- Regular performance benchmarking
- Security-first mindset for file handling
- **Phase 1 Complete**: All development environment setup verified and working

### Code Review Criteria
- Windows API usage follows best practices
- COM reference counting is correct
- Error handling is comprehensive
- Performance impact is measured
- Tests cover new functionality

---

**Document Version**: 1.0  
**Last Updated**: July 21, 2025  
**Next Review**: End of each development phase  
**Maintainer**: Rohit

*This document serves as the authoritative source for all technical decisions, rationale, and project context. Update whenever significant decisions are made.*