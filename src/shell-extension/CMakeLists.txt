# Shell Extension Module - COM DLL Implementation

# Shell extension source files
set(SHELL_EXT_SOURCES
    ArchiveExtractor.cpp
    ArchiveExtractor.h
    ClassFactory.cpp
    dllmain.cpp
    Registry.cpp
    ArchiveExtractorShell.def
)

# Create the shell extension DLL
add_library(ArchiveExtractorShell SHARED ${SHELL_EXT_SOURCES})

# Set target properties
set_target_properties(ArchiveExtractorShell PROPERTIES
    OUTPUT_NAME "ArchiveExtractorShell"
    SUFFIX ".dll"
    WINDOWS_EXPORT_ALL_SYMBOLS FALSE
)

# Link required Windows libraries
target_link_libraries(ArchiveExtractorShell PRIVATE 
    comctl32
    ole32
    oleaut32
    uuid
    shlwapi
    shell32
    user32
    kernel32
)

# Set compile definitions
target_compile_definitions(ArchiveExtractorShell PRIVATE
    _WINDOWS
    _USRDLL
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
)

# Set runtime output directory
set_target_properties(ArchiveExtractorShell PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)