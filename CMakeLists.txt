cmake_minimum_required(VERSION 3.20)
project(WindowsArchiveExtractor VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Set Windows-specific settings
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Find required packages
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(src)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Main shell extension DLL
set(SHELL_SOURCES
    src/shell-extension/ArchiveExtractor.cpp
    src/shell-extension/ArchiveExtractor.h
    src/shell-extension/ClassFactory.cpp
    src/shell-extension/ClassFactory.h
    src/shell-extension/dllmain.cpp
    src/shell-extension/resource.rc
)

# Create the shell extension DLL target (placeholder - will be populated in Phase 2)
# add_library(ArchiveExtractorShell SHARED ${SHELL_SOURCES})

# Uncomment when source files are created:
# target_link_libraries(ArchiveExtractorShell PRIVATE 
#     ZLIB::ZLIB 
#     BZip2::BZip2
#     comctl32
#     ole32
#     oleaut32
#     uuid
#     shlwapi
# )

# Set DLL properties
# set_target_properties(ArchiveExtractorShell PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#     OUTPUT_NAME "ArchiveExtractorShell"
# )

# Installation
install(DIRECTORY resources/ DESTINATION share/windows-archive-extractor)

# CPack configuration for installer
set(CPACK_PACKAGE_NAME "Windows Archive Extractor")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Windows Shell Extension for extracting compressed archives")
set(CPACK_GENERATOR "NSIS")
include(CPack)