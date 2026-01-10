# PowerShell script to extract SonarQube Scanner for Windows
# Extracts sonar-scanner.zip to sonar-scanner-7.3.0.5189-windows-x64 directory
# Extracts build-wrapper-win-x86.zip to build-wrapper directory
#
# To run this script, use one of the following methods:
# 1. Bypass execution policy for this session:
#    powershell -ExecutionPolicy Bypass -File .\sonar\windows\extract.ps1
# 2. Or run from PowerShell with bypass:
#    Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
#    .\sonar\windows\extract.ps1

$scriptDir = $PSScriptRoot
$zipFileName = "sonar-scanner.zip"
$zipFilePath = Join-Path $scriptDir $zipFileName

# Alternative zip file names to check
$alternativeZipNames = @(
    "sonar-scanner-cli-7.3.0.5189-windows-x64.zip",
    "sonar-scanner.zip"
)

# Find the zip file
$zipFile = $null
foreach ($name in $alternativeZipNames) {
    $path = Join-Path $scriptDir $name
    if (Test-Path $path) {
        $zipFile = $path
        $zipFileName = $name
        break
    }
}

if (-not $zipFile) {
    Write-Host "Warning: Sonar scanner zip file not found in: $scriptDir" -ForegroundColor Yellow
    Write-Host "Expected file names:" -ForegroundColor Yellow
    foreach ($name in $alternativeZipNames) {
        Write-Host "  - $name" -ForegroundColor Yellow
    }
    Write-Host "Skipping sonar-scanner extraction. Continuing with build-wrapper extraction..." -ForegroundColor Yellow
    $skipSonarScanner = $true
} else {
    $skipSonarScanner = $false
}

if (-not $skipSonarScanner) {
    Write-Host "=== Extracting Sonar Scanner ===" -ForegroundColor Cyan
    Write-Host "Found zip file: $zipFile" -ForegroundColor Green
    
    # Determine extraction directory (extract to a folder with the same name as zip without extension)
    $extractDirName = [System.IO.Path]::GetFileNameWithoutExtension($zipFileName)
    $extractDir = Join-Path $scriptDir $extractDirName
    
    # Check if scanner directories already exist in the parent directory
    $scannerDirs = @("bin", "conf", "jre", "lib")
    $existingDirs = @()
    foreach ($dir in $scannerDirs) {
        $dirPath = Join-Path $scriptDir $dir
        if (Test-Path $dirPath) {
            $existingDirs += $dir
        }
    }
    
    if ($existingDirs.Count -gt 0) {
        Write-Host "Scanner directories already exist: $($existingDirs -join ', ')" -ForegroundColor Yellow
        $response = Read-Host "Do you want to extract again? This will overwrite existing files. (y/n)"
        if ($response -ne "y" -and $response -ne "Y") {
            Write-Host "Sonar scanner extraction cancelled. Continuing with build-wrapper extraction..." -ForegroundColor Yellow
            $skipSonarScanner = $true
        } else {
            Write-Host "Removing existing scanner directories..." -ForegroundColor Yellow
            foreach ($dir in $existingDirs) {
                $dirPath = Join-Path $scriptDir $dir
                if (Test-Path $dirPath) {
                    Remove-Item -Path $dirPath -Recurse -Force
                }
            }
        }
    }
    
    if (-not $skipSonarScanner) {
        Write-Host "Extracting to: $extractDir" -ForegroundColor Cyan
        
        try {
            # Check if .NET 4.5+ is available (for Expand-Archive)
            $netVersion = [System.Environment]::Version
            if ($netVersion.Major -ge 4) {
                # Use PowerShell's built-in Expand-Archive (requires PowerShell 5.0+)
                if (Get-Command Expand-Archive -ErrorAction SilentlyContinue) {
                    Write-Host "Using Expand-Archive..." -ForegroundColor Cyan
                    Expand-Archive -Path $zipFile -DestinationPath $extractDir -Force
                } else {
                    # Fallback to .NET methods for older PowerShell versions
                    Write-Host "Using .NET System.IO.Compression..." -ForegroundColor Cyan
                    Add-Type -AssemblyName System.IO.Compression.FileSystem
                    [System.IO.Compression.ZipFile]::ExtractToDirectory($zipFile, $extractDir)
                }
            } else {
                # Use .NET methods directly
                Write-Host "Using .NET System.IO.Compression..." -ForegroundColor Cyan
                Add-Type -AssemblyName System.IO.Compression.FileSystem
                [System.IO.Compression.ZipFile]::ExtractToDirectory($zipFile, $extractDir)
            }
            
            if (Test-Path $extractDir) {
                $itemCount = (Get-ChildItem -Path $extractDir -Recurse -File).Count
                Write-Host "Extraction completed successfully!" -ForegroundColor Green
                Write-Host "Extracted $itemCount files to: $extractDir" -ForegroundColor Green
                
                # Find the sonar-scanner-7.3.0.5189-windows-x64 directory
                $scannerSubDir = Join-Path $extractDir "sonar-scanner-7.3.0.5189-windows-x64"
                
                if (Test-Path $scannerSubDir) {
                    Write-Host "Moving contents from $scannerSubDir to parent directory..." -ForegroundColor Cyan
                    
                    # Get all items in the scanner subdirectory
                    $items = Get-ChildItem -Path $scannerSubDir -Force
                    
                    foreach ($item in $items) {
                        $destination = Join-Path $scriptDir $item.Name
                        
                        # If destination already exists, remove it first
                        if (Test-Path $destination) {
                            Write-Host "Removing existing: $destination" -ForegroundColor Yellow
                            Remove-Item -Path $destination -Recurse -Force
                        }
                        
                        # Move the item to parent directory
                        Write-Host "Moving: $($item.Name)" -ForegroundColor Cyan
                        Move-Item -Path $item.FullName -Destination $scriptDir -Force
                    }
                    
                    # Remove the now-empty sonar-scanner-7.3.0.5189-windows-x64 directory
                    Write-Host "Removing empty directory: $scannerSubDir" -ForegroundColor Cyan
                    Remove-Item -Path $scannerSubDir -Force -ErrorAction SilentlyContinue
                    
                    # Also remove the extract directory if it's now empty
                    $remainingItems = Get-ChildItem -Path $extractDir -Force -ErrorAction SilentlyContinue
                    if ($remainingItems.Count -eq 0) {
                        Write-Host "Removing empty extraction directory: $extractDir" -ForegroundColor Cyan
                        Remove-Item -Path $extractDir -Force -ErrorAction SilentlyContinue
                    }
                    
                    Write-Host "Contents moved successfully to: $scriptDir" -ForegroundColor Green
                    
                    # Show the main scanner executable path if it exists
                    $scannerExe = Join-Path $scriptDir "bin\sonar-scanner.bat"
                    if (Test-Path $scannerExe) {
                        Write-Host "Scanner executable found at: $scannerExe" -ForegroundColor Green
                    } else {
                        # Try to find any sonar-scanner.bat in subdirectories
                        $foundScanner = Get-ChildItem -Path $scriptDir -Filter "sonar-scanner.bat" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
                        if ($foundScanner) {
                            Write-Host "Scanner executable found at: $($foundScanner.FullName)" -ForegroundColor Green
                        }
                    }
                } else {
                    Write-Host "Warning: sonar-scanner-7.3.0.5189-windows-x64 directory not found in extraction directory." -ForegroundColor Yellow
                    Write-Host "Contents remain in: $extractDir" -ForegroundColor Yellow
                }
            } else {
                Write-Host "Error: Extraction directory was not created." -ForegroundColor Red
                Write-Host "Continuing with build-wrapper extraction..." -ForegroundColor Yellow
            }
        } catch {
            Write-Host "Error extracting sonar scanner: $_" -ForegroundColor Red
            Write-Host "Stack trace: $($_.ScriptStackTrace)" -ForegroundColor Red
            Write-Host "Continuing with build-wrapper extraction..." -ForegroundColor Yellow
        }
    }
}

# Extract Build Wrapper
Write-Host ""
Write-Host "=== Extracting Build Wrapper ===" -ForegroundColor Cyan

$buildWrapperZipFileName = "build-wrapper-win-x86.zip"
$buildWrapperZipFilePath = Join-Path $scriptDir $buildWrapperZipFileName

if (-not (Test-Path $buildWrapperZipFilePath)) {
    Write-Host "Warning: Build wrapper zip file not found: $buildWrapperZipFilePath" -ForegroundColor Yellow
    Write-Host "Skipping build wrapper extraction." -ForegroundColor Yellow
} else {
    Write-Host "Found build wrapper zip file: $buildWrapperZipFilePath" -ForegroundColor Green
    
    $buildWrapperExtractDir = Join-Path $scriptDir "build-wrapper"
    
    # Check if build-wrapper directory already exists
    if (Test-Path $buildWrapperExtractDir) {
        Write-Host "Build wrapper directory already exists: $buildWrapperExtractDir" -ForegroundColor Yellow
        $response = Read-Host "Do you want to extract again? This will overwrite existing files. (y/n)"
        if ($response -ne "y" -and $response -ne "Y") {
            Write-Host "Build wrapper extraction cancelled." -ForegroundColor Yellow
        } else {
            Write-Host "Removing existing build-wrapper directory..." -ForegroundColor Yellow
            Remove-Item -Path $buildWrapperExtractDir -Recurse -Force
        }
    }
    
    if (-not (Test-Path $buildWrapperExtractDir)) {
        Write-Host "Extracting build wrapper directly to: $scriptDir" -ForegroundColor Cyan
        
        try {
            # Check if .NET 4.5+ is available (for Expand-Archive)
            $netVersion = [System.Environment]::Version
            if ($netVersion.Major -ge 4) {
                # Use PowerShell's built-in Expand-Archive (requires PowerShell 5.0+)
                if (Get-Command Expand-Archive -ErrorAction SilentlyContinue) {
                    Write-Host "Using Expand-Archive..." -ForegroundColor Cyan
                    Expand-Archive -Path $buildWrapperZipFilePath -DestinationPath $scriptDir -Force
                } else {
                    # Fallback to .NET methods for older PowerShell versions
                    Write-Host "Using .NET System.IO.Compression..." -ForegroundColor Cyan
                    Add-Type -AssemblyName System.IO.Compression.FileSystem
                    [System.IO.Compression.ZipFile]::ExtractToDirectory($buildWrapperZipFilePath, $scriptDir)
                }
            } else {
                # Use .NET methods directly
                Write-Host "Using .NET System.IO.Compression..." -ForegroundColor Cyan
                Add-Type -AssemblyName System.IO.Compression.FileSystem
                [System.IO.Compression.ZipFile]::ExtractToDirectory($buildWrapperZipFilePath, $scriptDir)
            }
            
            # Assume the extracted folder is build-wrapper-win-x86 and rename it to build-wrapper
            $extractedFolderPath = Join-Path $scriptDir "build-wrapper-win-x86"
            
            if (Test-Path $extractedFolderPath) {
                Write-Host "Found extracted folder: build-wrapper-win-x86" -ForegroundColor Cyan
                Write-Host "Renaming to: build-wrapper" -ForegroundColor Cyan
                
                # Rename the extracted folder to build-wrapper
                try {
                    Rename-Item -Path $extractedFolderPath -NewName "build-wrapper" -Force -ErrorAction Stop
                } catch {
                    Write-Host "Error renaming folder: $_" -ForegroundColor Red
                    Write-Host "Source path: $extractedFolderPath" -ForegroundColor Red
                    throw
                }
                
                if (Test-Path $buildWrapperExtractDir) {
                    $itemCount = (Get-ChildItem -Path $buildWrapperExtractDir -Recurse -File).Count
                    Write-Host "Build wrapper extraction completed successfully!" -ForegroundColor Green
                    Write-Host "Extracted $itemCount files to: $buildWrapperExtractDir" -ForegroundColor Green
                    
                    # Show the build-wrapper executable path if it exists
                    $foundWrapper = Get-ChildItem -Path $buildWrapperExtractDir -Filter "build-wrapper*.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
                    if ($foundWrapper) {
                        Write-Host "Build wrapper executable found at: $($foundWrapper.FullName)" -ForegroundColor Green
                    }
                } else {
                    Write-Host "Error: Build wrapper directory was not renamed successfully." -ForegroundColor Red
                }
            } else {
                Write-Host "Warning: Extracted folder 'build-wrapper-win-x86' not found at: $extractedFolderPath" -ForegroundColor Yellow
            }
        } catch {
            Write-Host "Error extracting build wrapper: $_" -ForegroundColor Red
            Write-Host "Stack trace: $($_.ScriptStackTrace)" -ForegroundColor Red
        }
    }
}

Write-Host ""
Write-Host "All extractions completed!" -ForegroundColor Green

