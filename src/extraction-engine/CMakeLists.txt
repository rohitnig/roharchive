# Extraction Engine Module - Phase 3 Implementation

# Static library for extraction functionality
set(EXTRACTION_ENGINE_SOURCES
    ArchiveExtractor.h
    Utils.cpp
    TarExtractor.cpp
    TarExtractor.h
    ArchiveExtractorFactory.cpp
)

add_library(ExtractionEngine STATIC ${EXTRACTION_ENGINE_SOURCES})

# Set target properties
target_compile_definitions(ExtractionEngine PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
)

# Include directories
target_include_directories(ExtractionEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link required libraries
target_link_libraries(ExtractionEngine PRIVATE 
    # TODO: Add compression libraries when vcpkg is working
    # ZLIB::ZLIB 
    # BZip2::BZip2
)