# Phase 4 Integration Testing Script - Simple Version
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

# Test 2: Create test archives
Write-Host "`nTest 2: Creating Test Archives" -ForegroundColor Cyan

# Copy existing test archive
$sourceArchive = "C:\Users\rohit\Documents\Sources\roharchive\test-files\demo.tar"
$testArchive1 = "$TestPath\test_archive1.tar"
$testArchive2 = "$TestPath\test_archive2.tar"

if (Test-Path $sourceArchive) {
    Copy-Item $sourceArchive $testArchive1
    Copy-Item $sourceArchive $testArchive2
    Write-Host "  ✓ Test archives created successfully" -ForegroundColor Green
    Write-Host "    - $testArchive1"
    Write-Host "    - $testArchive2"
} else {
    Write-Warning "  Source archive not found: $sourceArchive"
    # Create dummy archives
    "TAR TEST FILE" | Out-File $testArchive1 -Encoding ASCII
    "TAR TEST FILE" | Out-File $testArchive2 -Encoding ASCII
    Write-Host "  ⚠ Created dummy test archives" -ForegroundColor Yellow
}

# Test 3: Manual testing instructions
Write-Host "`nTest 3: Manual Context Menu Testing" -ForegroundColor Cyan
Write-Host "  Manual testing steps:" -ForegroundColor Yellow
Write-Host "  1. Open Windows Explorer"
Write-Host "  2. Navigate to: $TestPath"
Write-Host "  3. Right-click on test_archive1.tar"
Write-Host "  4. Verify 'Extract Archive' submenu appears with options:"
Write-Host "     • Extract Here"
Write-Host "     • Extract to Folder..."
Write-Host "     • Extract to Subfolder"
Write-Host "     • Extract and Delete"
Write-Host "     • Test Archive"
Write-Host "  5. Select both archives and test multi-file context menu"
Write-Host "  6. Test each extraction option"

# Test Summary
Write-Host "`n=== Phase 4 Integration Test Complete ===" -ForegroundColor Green
Write-Host ""
Write-Host "Automated Tests:" -ForegroundColor Yellow
Write-Host "  ✓ DLL Build and Registration"
Write-Host "  ✓ Test Archive Preparation"
Write-Host ""
Write-Host "Manual Tests Required:" -ForegroundColor Yellow
Write-Host "  • Context menu integration verification"
Write-Host "  • Multi-file extraction workflow testing"
Write-Host "  • Error handling validation"
Write-Host ""
Write-Host "Test Location: $TestPath" -ForegroundColor Cyan
Write-Host ""
Write-Host "DLL remains registered for manual testing." -ForegroundColor Green
Write-Host "To unregister: regsvr32 /u `"$dllPath`""