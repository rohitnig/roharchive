# Test configuration
include(GoogleTest)

# Test data directory
set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test-data")

# Unit tests for extraction engine
# set(EXTRACTION_ENGINE_TEST_SOURCES
#     test_gzip_extractor.cpp
#     test_bzip2_extractor.cpp
#     test_tar_extractor.cpp
# )

# add_executable(extraction_engine_tests ${EXTRACTION_ENGINE_TEST_SOURCES})
# target_link_libraries(extraction_engine_tests PRIVATE
#     GTest::gtest_main
#     ExtractionEngine
# )
# gtest_discover_tests(extraction_engine_tests)

# Integration tests for shell extension
# set(SHELL_EXTENSION_TEST_SOURCES
#     test_shell_extension.cpp
#     test_context_menu.cpp
# )

# add_executable(shell_extension_tests ${SHELL_EXTENSION_TEST_SOURCES})
# target_link_libraries(shell_extension_tests PRIVATE
#     GTest::gtest_main
#     ArchiveExtractorShell
# )
# gtest_discover_tests(shell_extension_tests)

# Create test data directory
file(MAKE_DIRECTORY ${TEST_DATA_DIR})