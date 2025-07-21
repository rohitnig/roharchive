# Windows Archive Extractor - Test Cases

## Test Strategy Overview

**Testing Approach**: Comprehensive unit, integration, and user acceptance testing
**Test Framework**: Google Test (gtest) integrated with VS Code Test Explorer
**Coverage Goal**: >90% code coverage for core functionality
**Test Environment**: Windows 10/11 with various archive types and sizes

## Unit Test Categories

### 1. Compression Library Tests

#### 1.1 Gzip Extraction Tests
```cpp
class GzipExtractorTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    GzipExtractor extractor;
    std::string test_data_dir;
};
```

**Test Cases:**
- `ExtractValidGzipFile` - Extract standard .gz file
- `ExtractEmptyGzipFile` - Handle empty compressed files
- `ExtractLargeGzipFile` - Test with >100MB files
- `ExtractCorruptedGzipFile` - Handle malformed files
- `ExtractGzipWithUnicodeNames` - Unicode filename support
- `ExtractGzipToReadOnlyLocation` - Permission error handling
- `ExtractGzipWithInsufficientSpace` - Disk space validation

#### 1.2 Bzip2 Extraction Tests
**Test Cases:**
- `ExtractValidBzip2File` - Extract standard .bz2 file
- `ExtractBzip2WithLongFilename` - Handle long filenames
- `ExtractMultipleBzip2Files` - Batch processing
- `ExtractBzip2WithSpecialCharacters` - Special characters in paths
- `ExtractBzip2ProgressCallback` - Progress reporting accuracy
- `CancelBzip2Extraction` - User cancellation handling

#### 1.3 Tar Archive Tests
**Test Cases:**
- `ExtractSimpleTarArchive` - Basic tar extraction
- `ExtractTarWithDirectories` - Nested directory structure
- `ExtractTarWithSymlinks` - Symbolic link handling (skip)
- `ExtractTarWithLongPaths` - Windows path length limits
- `ExtractTarWithFilePermissions` - Permission preservation
- `ExtractPartialTarArchive` - Incomplete/truncated archives

#### 1.4 Compound Format Tests
**Test Cases:**
- `ExtractTarGzipArchive` - .tar.gz double compression
- `ExtractTarBzip2Archive` - .tar.bz2 double compression
- `AutoDetectCompressionFormat` - Format detection
- `ExtractNestedArchives` - Archive within archive
- `ExtractMixedFormatSelection` - Multiple different formats

### 2. Shell Extension Tests

#### 2.1 COM Interface Tests
```cpp
class ShellExtensionTest : public ::testing::Test {
protected:
    void SetUp() override;
    Microsoft::WRL::ComPtr<IContextMenu> context_menu;
    Microsoft::WRL::ComPtr<IShellExtInit> shell_init;
};
```

**Test Cases:**
- `CreateContextMenuInterface` - COM object creation
- `InitializeWithDataObject` - File selection handling
- `QueryContextMenuSingleFile` - Single file context menu
- `QueryContextMenuMultipleFiles` - Multiple file selection
- `InvokeExtractionCommand` - Command execution
- `GetCommandStringInfo` - Menu text and help strings
- `HandleUnsupportedFileTypes` - Non-archive file filtering

#### 2.2 Registry Integration Tests
**Test Cases:**
- `RegisterShellExtension` - COM registration success
- `UnregisterShellExtension` - Clean unregistration
- `VerifyFileAssociations` - File type associations
- `CheckApprovedExtensionsList` - Security approval list
- `ValidateClassIdGeneration` - Unique CLSID creation

### 3. UI Component Tests

#### 3.1 Progress Dialog Tests
**Test Cases:**
- `ShowProgressDialog` - Dialog creation and display
- `UpdateProgressPercentage` - Progress bar updates
- `UpdateProgressText` - Status message updates
- `CancelExtractionOperation` - Cancel button functionality
- `HandleProgressDialogClose` - Window close handling
- `ProgressDialogMemoryLeaks` - Resource cleanup verification

#### 3.2 Error Dialog Tests
**Test Cases:**
- `ShowFileNotFoundError` - Missing file error display
- `ShowPermissionDeniedError` - Access denied handling
- `ShowCorruptedArchiveError` - Invalid archive notification
- `ShowInsufficientSpaceError` - Disk space error
- `ShowGenericExtractionError` - Fallback error handling

## Integration Test Categories

### 4. End-to-End Workflow Tests

#### 4.1 Windows Explorer Integration
**Test Environment**: Real Windows Explorer process

**Test Cases:**
- `RightClickSingleGzipFile` - Context menu appears correctly
- `RightClickMultipleArchives` - Multi-selection handling
- `ExtractToCurrentFolder` - Default extraction location
- `ExtractToSubfolder` - Subfolder creation and extraction
- `ExtractWithFileConflicts` - Overwrite/rename handling
- `ExtractLargeArchiveWithProgress` - Progress dialog integration
- `CancelExtractionMidProcess` - Cancellation workflow
- `ExtractMultipleArchivesSequentially` - Batch processing

#### 4.2 File System Integration
**Test Cases:**
- `ExtractToNetworkDrive` - UNC path support
- `ExtractWithLongPaths` - >260 character path handling
- `ExtractToExFATFileSystem` - Different file system support
- `ExtractWithReadOnlyFiles` - File attribute preservation
- `ExtractWithHiddenFiles` - Hidden file extraction
- `HandleFileSystemErrors` - I/O error recovery

### 5. Performance Tests

#### 5.1 Extraction Performance
**Test Cases:**
- `ExtractLargeGzipFile_Performance` - >1GB file extraction time
- `ExtractManySmallFiles_Performance` - Many small files efficiency
- `ExtractTarArchive_MemoryUsage` - Memory consumption validation
- `ExtractMultipleArchives_ConcurrentProcessing` - Multi-threading
- `ExtractWithProgressCallbacks_Overhead` - Callback performance impact

#### 5.2 Shell Extension Performance
**Test Cases:**
- `ContextMenuDisplayTime` - Menu appearance speed
- `FileSelectionHandling_Performance` - Large selection processing
- `ExtensionLoadTime` - DLL loading performance
- `MemoryUsageWhenIdle` - Background memory consumption

### 6. Stress Tests

#### 6.1 Resource Exhaustion Tests
**Test Cases:**
- `ExtractWithLowMemory` - Memory pressure handling
- `ExtractWithLowDiskSpace` - Disk space monitoring
- `ExtractManyArchivesSimultaneously` - Resource contention
- `ExtractVeryLargeArchive` - >4GB archive support
- `ExtractArchiveWithManyFiles` - >100k files in archive

#### 6.2 Error Condition Tests
**Test Cases:**
- `ExtractCorruptedArchiveHeader` - Header corruption
- `ExtractArchiveWithCorruptedData` - Data corruption
- `ExtractArchiveWithMissingData` - Truncated archive
- `ExtractDuringSystemShutdown` - Graceful shutdown handling
- `ExtractWithAntivirusInterference` - AV software interaction

## User Acceptance Test Scenarios

### 7. Real-World Usage Tests

#### 7.1 Typical User Workflows
**Test Scenarios:**
1. **Download and Extract**
   - Download .tar.gz from web
   - Right-click → Extract
   - Verify extracted contents
   - Clean up temporary files

2. **Batch Processing**
   - Select multiple different archive types
   - Right-click → Extract All
   - Monitor progress for all extractions
   - Verify all extractions completed

3. **Error Recovery**
   - Attempt to extract corrupted archive
   - Receive clear error message
   - Continue with other valid archives
   - No system instability

4. **Large File Handling**
   - Extract >500MB archive
   - Progress indication works smoothly
   - Cancel mid-extraction
   - Verify partial cleanup

#### 7.2 Edge Case Scenarios
**Test Scenarios:**
1. **Special Characters**
   - Archive with Unicode filenames
   - Extract to path with spaces
   - Handle international characters
   - Preserve original names

2. **Network Scenarios**
   - Extract archive from network drive
   - Extract to network location
   - Handle network interruptions
   - Timeout and retry logic

3. **Multi-User Environment**
   - Multiple users using extension
   - Different user permissions
   - Shared archive access
   - User-specific settings

## Test Data Requirements

### 8. Test Archive Collection

#### 8.1 Positive Test Archives
```
test-archives/
├── small/
│   ├── empty.txt.gz          # 0 bytes content
│   ├── single-file.txt.gz    # 1KB content
│   ├── simple.tar            # 5KB, 3 files
│   └── basic.tar.bz2         # 2KB compressed
├── medium/
│   ├── documents.tar.gz      # 50MB, mixed file types
│   ├── source-code.tar.bz2   # 25MB, many small files
│   └── images.tar            # 100MB, binary files
├── large/
│   ├── database.tar.gz       # 500MB, single large file
│   └── logs.tar.bz2          # 1GB, many log files
└── unicode/
    ├── 中文文件.txt.gz         # Chinese characters
    ├── émoji🎉.tar.gz          # Special characters
    └── длинное_имя.tar.bz2     # Cyrillic characters
```

#### 8.2 Negative Test Archives
```
corrupted-archives/
├── truncated.tar.gz          # Incomplete archive
├── wrong-header.bz2          # Invalid header
├── empty-file.gz             # Zero-length file
├── corrupted-data.tar        # Data corruption
└── infinite-loop.tar         # Malicious archive structure
```

## Test Automation

### 9. Continuous Integration Tests

#### 9.1 Build Verification Tests
```yaml
# CI pipeline test stages
stages:
  - build_debug
  - build_release  
  - unit_tests
  - integration_tests
  - performance_tests
  - security_tests
```

**Automated Test Execution:**
- Run on every commit
- Parallel test execution
- Test result reporting
- Code coverage analysis
- Performance regression detection

#### 9.2 Nightly Test Suite
**Extended Test Scenarios:**
- Full performance test suite
- Large archive processing tests
- Memory leak detection runs
- Compatibility testing on different Windows versions
- Security vulnerability scanning

## Test Reporting

### 10. Test Metrics and Reporting

#### 10.1 Coverage Metrics
- **Line Coverage**: Target >90%
- **Branch Coverage**: Target >85%  
- **Function Coverage**: Target >95%
- **Integration Coverage**: All user workflows covered

#### 10.2 Performance Benchmarks
```cpp
// Example performance test structure
TEST(PerformanceTest, ExtractLargeGzipFile) {
    auto start = std::chrono::high_resolution_clock::now();
    
    EXPECT_TRUE(extractor.Extract("large-file.gz", "output/"));
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    EXPECT_LT(duration.count(), 30); // Should complete in <30 seconds
}
```

#### 10.3 Test Execution Reports
- **Test Results Dashboard**: Pass/fail rates, execution times
- **Code Coverage Reports**: Detailed coverage analysis
- **Performance Trends**: Execution time trends over builds
- **Error Analysis**: Common failure patterns and root causes

---

**Document Version**: 1.0  
**Last Updated**: July 21, 2025  
**Test Framework**: Google Test with VS Code integration  
**Coverage Target**: >90% for core functionality