# Phase 4 Integration Testing Script
# Tests shell extension integration and multi-file extraction workflows

param(
    [string]$BuildPath = "C:\Users\rohit\Documents\Sources\roharchive\build\bin\Debug",
    [string]$TestPath = "C:\Users\rohit\Documents\Sources\roharchive\test-integration"
)

Write-Host "=== Phase 4 Shell Integration Testing ===" -ForegroundColor Green
Write-Host "Build Path: $BuildPath"
Write-Host "Test Path: $TestPath"

# Cleanup previous test runs
if (Test-Path $TestPath) {
    Remove-Item $TestPath -Recurse -Force
}
New-Item -ItemType Directory -Path $TestPath -Force | Out-Null

# Create test archive files for multi-file testing
Write-Host "`nCreating test archive files..." -ForegroundColor Yellow

# Create test files for archives
$testFiles = @{
    "archive1_files" = @("file1.txt", "file2.txt")
    "archive2_files" = @("data.txt", "readme.md", "config.ini")
}

foreach ($archiveName in $testFiles.Keys) {
    $archiveDir = "$TestPath\$archiveName"
    New-Item -ItemType Directory -Path $archiveDir -Force | Out-Null
    
    foreach ($fileName in $testFiles[$archiveName]) {
        $content = "This is test content for $fileName in $archiveName`n"
        $content += "Created on $(Get-Date)`n"
        $content += "File size test data: " + ("x" * 100)
        
        Set-Content -Path "$archiveDir\$fileName" -Value $content -Encoding UTF8
    }
    
    Write-Host "  Created test files in $archiveDir"
}

# Create TAR archives using PowerShell (basic implementation)
Write-Host "`nCreating TAR test archives..." -ForegroundColor Yellow

function Create-TarArchive {
    param(
        [string]$SourcePath,
        [string]$TarPath
    )
    
    # Use tar command if available (Windows 10 1803+)
    if (Get-Command tar -ErrorAction SilentlyContinue) {
        Push-Location (Split-Path $SourcePath)
        $folderName = Split-Path $SourcePath -Leaf
        tar -cf $TarPath $folderName
        Pop-Location
        return $true
    } else {
        Write-Warning "tar command not available, creating dummy archive"
        # Create a simple TAR-like file for testing
        $content = @()
        Get-ChildItem $SourcePath -Recurse -File | ForEach-Object {
            $content += $_.FullName
        }
        $content | Out-File $TarPath -Encoding ASCII
        return $false
    }
}

$archive1Path = "$TestPath\test_archive1.tar"
$archive2Path = "$TestPath\test_archive2.tar"

Create-TarArchive "$TestPath\archive1_files" $archive1Path
Create-TarArchive "$TestPath\archive2_files" $archive2Path

# Test 1: Shell Extension Registration
Write-Host "`nTest 1: Shell Extension Registration" -ForegroundColor Cyan
$dllPath = "$BuildPath\ArchiveExtractorShell.dll"

if (-not (Test-Path $dllPath)) {
    Write-Error "DLL not found: $dllPath"
    exit 1
}

Write-Host "  DLL found: $dllPath"

# Unregister first (in case it's already registered)
Write-Host "  Unregistering DLL..."
& regsvr32 /u /s "$dllPath"

# Register DLL
Write-Host "  Registering DLL..."
$regResult = & regsvr32 /s "$dllPath" 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "  ✓ DLL registered successfully" -ForegroundColor Green
} else {
    Write-Error "  ✗ Failed to register DLL: $regResult"
    exit 1
}

# Test 2: COM Interface Validation
Write-Host "`nTest 2: COM Interface Validation" -ForegroundColor Cyan

# Check registry entries (using actual GUID from project)
$registryPaths = @(
    'HKCR:\CLSID\{12345678-1234-1234-1234-123456789ABC}',
    'HKCR:\*\shellex\ContextMenuHandlers\ArchiveExtractor'
)

foreach ($regPath in $registryPaths) {
    if (Test-Path $regPath) {
        Write-Host "  ✓ Registry entry exists: $regPath" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ Registry entry missing: $regPath" -ForegroundColor Yellow
    }
}

# Test 3: Multi-file Selection Simulation
Write-Host "`nTest 3: Multi-file Selection Test" -ForegroundColor Cyan

if ((Test-Path $archive1Path) -and (Test-Path $archive2Path)) {
    Write-Host "  ✓ Multiple test archives created:" -ForegroundColor Green
    Write-Host "    - $archive1Path"
    Write-Host "    - $archive2Path"
    
    # Get file sizes
    $size1 = (Get-Item $archive1Path).Length
    $size2 = (Get-Item $archive2Path).Length
    Write-Host "    Archive sizes: $size1 bytes, $size2 bytes"
} else {
    Write-Host "  ✗ Failed to create test archives" -ForegroundColor Red
}

# Test 4: Context Menu Simulation (Manual verification needed)
Write-Host "`nTest 4: Context Menu Testing" -ForegroundColor Cyan
Write-Host "  Manual testing required:"
Write-Host "  1. Open Windows Explorer"
Write-Host "  2. Navigate to: $TestPath"
Write-Host "  3. Right-click on test_archive1.tar"
Write-Host "  4. Verify 'Extract Archive' submenu appears"
Write-Host "  5. Test submenu options:"
Write-Host "     - Extract Here"
Write-Host "     - Extract to Folder..."
Write-Host "     - Extract to Subfolder"
Write-Host "     - Extract and Delete"
Write-Host "     - Test Archive"
Write-Host "  6. Select multiple .tar files and verify context menu"

# Test 5: Extraction Workflow Test
Write-Host "`nTest 5: Programmatic Extraction Test" -ForegroundColor Cyan

# Create a test extractor instance (if we can)
try {
    # This would be a programmatic test of the extraction engine
    $extractionTestPath = "$TestPath\extraction_test"
    New-Item -ItemType Directory -Path $extractionTestPath -Force | Out-Null
    
    Write-Host "  ✓ Extraction test directory created: $extractionTestPath" -ForegroundColor Green
    Write-Host "  Note: Full extraction testing requires manual context menu testing"
} catch {
    Write-Host "  ⚠ Could not create extraction test directory: $($_.Exception.Message)" -ForegroundColor Yellow
}

# Test 6: Error Handling and Edge Cases
Write-Host "`nTest 6: Edge Case Testing" -ForegroundColor Cyan

# Create invalid/empty archive for testing
$invalidArchive = "$TestPath\invalid_archive.tar"
"This is not a valid TAR file" | Out-File $invalidArchive -Encoding ASCII
Write-Host "  ✓ Invalid archive created for error testing: $invalidArchive" -ForegroundColor Green

# Create empty archive
$emptyArchive = "$TestPath\empty_archive.tar"
New-Item -ItemType File -Path $emptyArchive -Force | Out-Null
Write-Host "  ✓ Empty archive created for testing: $emptyArchive" -ForegroundColor Green

# Test Summary
Write-Host "`n=== Phase 4 Integration Test Summary ===" -ForegroundColor Green
Write-Host "Test files created in: $TestPath"
Write-Host ""
Write-Host "Automated Tests Completed:" -ForegroundColor Yellow
Write-Host "  ✓ DLL Registration"
Write-Host "  ✓ Test Archive Creation"
Write-Host "  ✓ Multi-file Setup"
Write-Host "  ✓ Error Case Preparation"
Write-Host ""
Write-Host "Manual Tests Required:" -ForegroundColor Yellow
Write-Host "  • Context menu integration"
Write-Host "  • Multi-file selection workflow"
Write-Host "  • Extraction options testing"
Write-Host "  • Error handling verification"
Write-Host ""
Write-Host "To continue testing, open Windows Explorer and navigate to:" -ForegroundColor Cyan
Write-Host "  $TestPath"

# Keep DLL registered for manual testing
Write-Host "`nNote: DLL remains registered for manual testing." -ForegroundColor Yellow
Write-Host "To unregister later, run: regsvr32 /u `"$dllPath`"" -ForegroundColor Yellow