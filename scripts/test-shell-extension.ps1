# Test Shell Extension Script
# This script helps test the COM Shell Extension during development

param(
    [switch]$Register,
    [switch]$Unregister,
    [switch]$Test,
    [switch]$Rebuild
)

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$DllPath = "$ProjectRoot\build\bin\Debug\ArchiveExtractorShell.dll"
$TestFiles = "$ProjectRoot\test-files"

Write-Host "Archive Extractor Shell Extension Test Script" -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Cyan

function Build-Extension {
    Write-Host "Building shell extension..." -ForegroundColor Yellow
    Set-Location $ProjectRoot
    
    # Build the project
    cmake --build build --config Debug
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Build successful!" -ForegroundColor Green
        return $true
    } else {
        Write-Host "❌ Build failed!" -ForegroundColor Red
        return $false
    }
}

function Register-Extension {
    Write-Host "Registering shell extension..." -ForegroundColor Yellow
    
    if (-not (Test-Path $DllPath)) {
        Write-Host "❌ DLL not found at: $DllPath" -ForegroundColor Red
        return $false
    }
    
    try {
        Start-Process -FilePath "regsvr32.exe" -ArgumentList "/s", $DllPath -Verb RunAs -Wait
        Write-Host "✅ Shell extension registered successfully!" -ForegroundColor Green
        return $true
    }
    catch {
        Write-Host "❌ Failed to register shell extension: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

function Unregister-Extension {
    Write-Host "Unregistering shell extension..." -ForegroundColor Yellow
    
    try {
        Start-Process -FilePath "regsvr32.exe" -ArgumentList "/u", "/s", $DllPath -Verb RunAs -Wait
        Write-Host "✅ Shell extension unregistered successfully!" -ForegroundColor Green
        return $true
    }
    catch {
        Write-Host "❌ Failed to unregister shell extension: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

function Test-Extension {
    Write-Host "Testing shell extension..." -ForegroundColor Yellow
    
    if (-not (Test-Path $TestFiles)) {
        Write-Host "❌ Test files directory not found: $TestFiles" -ForegroundColor Red
        return $false
    }
    
    Write-Host "Opening test files directory in Explorer..." -ForegroundColor Yellow
    Write-Host "Right-click on any .gz, .bz2, .tar, .tar.gz, or .tar.bz2 file" -ForegroundColor Yellow
    Write-Host "You should see 'Extract Here' and 'Extract to Folder...' options" -ForegroundColor Yellow
    
    # Open Explorer to test files directory
    Start-Process "explorer.exe" -ArgumentList $TestFiles
    
    Write-Host "📝 Note: In Phase 2, extraction shows placeholder messages." -ForegroundColor Cyan
    Write-Host "   Actual extraction will be implemented in Phase 3." -ForegroundColor Cyan
    
    return $true
}

# Main script logic
if ($Rebuild) {
    Write-Host "Rebuilding and testing shell extension..." -ForegroundColor Magenta
    
    # Unregister first (ignore errors)
    Unregister-Extension | Out-Null
    
    # Build
    if (-not (Build-Extension)) {
        exit 1
    }
    
    # Register
    if (-not (Register-Extension)) {
        exit 1
    }
    
    # Test
    Test-Extension
}
elseif ($Register) {
    Register-Extension
}
elseif ($Unregister) {
    Unregister-Extension
}
elseif ($Test) {
    Test-Extension
}
else {
    Write-Host "Usage:" -ForegroundColor White
    Write-Host "  .\test-shell-extension.ps1 -Register    # Register the shell extension" -ForegroundColor Gray
    Write-Host "  .\test-shell-extension.ps1 -Unregister  # Unregister the shell extension" -ForegroundColor Gray
    Write-Host "  .\test-shell-extension.ps1 -Test        # Test the shell extension" -ForegroundColor Gray
    Write-Host "  .\test-shell-extension.ps1 -Rebuild     # Rebuild, register and test" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Example full test workflow:" -ForegroundColor Yellow
    Write-Host "  .\test-shell-extension.ps1 -Rebuild" -ForegroundColor Gray
}

Write-Host ""
Write-Host "Shell Extension Status:" -ForegroundColor Cyan
if (Test-Path $DllPath) {
    Write-Host "  DLL: ✅ Built" -ForegroundColor Green
    $dllInfo = Get-Item $DllPath
    Write-Host "  Last Modified: $($dllInfo.LastWriteTime)" -ForegroundColor Gray
} else {
    Write-Host "  DLL: ❌ Not built" -ForegroundColor Red
}