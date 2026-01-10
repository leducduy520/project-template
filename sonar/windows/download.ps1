# PowerShell script to download SonarQube Scanner for Windows
# Downloads sonar-scanner-cli-7.3.0.5189-windows-x64.zip and build-wrapper-win-x86.zip
#
# To run this script, use one of the following methods:
# 1. Bypass execution policy for this session:
#    powershell -ExecutionPolicy Bypass -File .\sonar\windows\download.ps1
# 2. Or run from PowerShell with bypass:
#    Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
#    .\sonar\windows\download.ps1

$outputDir = $PSScriptRoot

# Function to download a file
function Download-File {
    param(
        [string]$url,
        [string]$outputFileName
    )
    
    $outputFilePath = "$outputDir\$outputFileName"
    
    # Check if file already exists
    if (Test-Path $outputFilePath) {
        Write-Host "File already exists: $outputFilePath" -ForegroundColor Yellow
        $response = Read-Host "Do you want to download again? (y/n)"
        if ($response -ne "y" -and $response -ne "Y") {
            Write-Host "Download cancelled for $outputFileName." -ForegroundColor Yellow
            return $false
        }
        Remove-Item $outputFilePath -Force
    }
    
    Write-Host "Downloading from: $url" -ForegroundColor Cyan
    Write-Host "Saving to: $outputFilePath" -ForegroundColor Cyan
    
    # Check if Internet Download Manager (IDM) is installed
    $idmPaths = @(
        "${env:ProgramFiles(x86)}\Internet Download Manager\IDMan.exe",
        "${env:ProgramFiles}\Internet Download Manager\IDMan.exe",
        "$env:LOCALAPPDATA\Programs\Internet Download Manager\IDMan.exe"
    )
    
    $idmPath = $null
    foreach ($path in $idmPaths) {
        if (Test-Path $path) {
            $idmPath = $path
            break
        }
    }
    
    $useIdm = $false
    if ($idmPath) {
        Write-Host "Internet Download Manager detected at: $idmPath" -ForegroundColor Green
        $useIdm = $true
    } else {
        Write-Host "Internet Download Manager not found. Using PowerShell's Invoke-WebRequest." -ForegroundColor Yellow
    }
    
    try {
        if ($useIdm) {
            # Use IDM command-line interface
            Write-Host "Starting download with Internet Download Manager..." -ForegroundColor Cyan
            
            # IDM command-line syntax: /d URL /f OutputPath /n (start immediately)
            $idmArgs = @(
                "/d", $url,
                "/p", $outputDir,
                "/f", $outputFileName,
                "/n",
                "/q"
            )
            
            $process = Start-Process -FilePath $idmPath -ArgumentList $idmArgs -PassThru -NoNewWindow -Wait
            
            if ($process.ExitCode -ne 0) {
                Write-Host "IDM command failed with exit code: $($process.ExitCode)" -ForegroundColor Red
                Write-Host "Falling back to Invoke-WebRequest..." -ForegroundColor Yellow
                $useIdm = $false
            } else {
                # Wait for download to complete (IDM downloads are asynchronous)
                Write-Host "Waiting for download to complete..." -ForegroundColor Cyan
                $maxWaitTime = 300 # 5 minutes
                $waitInterval = 2 # Check every 2 seconds
                $elapsed = 0
                $initialSize = 0
                
                while ($elapsed -lt $maxWaitTime) {
                    if (Test-Path $outputFilePath) {
                        $currentSize = (Get-Item $outputFilePath).Length
                        # If file size hasn't changed in 10 seconds, assume download is complete
                        if ($currentSize -eq $initialSize -and $initialSize -gt 0) {
                            Start-Sleep -Seconds $waitInterval
                            $checkSize = (Get-Item $outputFilePath).Length
                            if ($checkSize -eq $currentSize) {
                                break
                            }
                        }
                        $initialSize = $currentSize
                    }
                    Start-Sleep -Seconds $waitInterval
                    $elapsed += $waitInterval
                }
            }
        }
        
        # Use Invoke-WebRequest if IDM is not available or failed
        if (-not $useIdm) {
            Write-Host "Downloading with Invoke-WebRequest..." -ForegroundColor Cyan
            Invoke-WebRequest -Uri $url -OutFile $outputFilePath -UseBasicParsing
        }
        
        # Verify download completed
        if (Test-Path $outputFilePath) {
            $fileSize = (Get-Item $outputFilePath).Length / 1MB
            if ($fileSize -gt 0) {
                Write-Host "Download completed successfully!" -ForegroundColor Green
                Write-Host "File size: $([math]::Round($fileSize, 2)) MB" -ForegroundColor Green
                Write-Host "File saved to: $outputFilePath" -ForegroundColor Green
                return $true
            } else {
                Write-Host "Error: Downloaded file is empty." -ForegroundColor Red
                return $false
            }
        } else {
            Write-Host "Error: File was not downloaded." -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Host "Error downloading file: $_" -ForegroundColor Red
        return $false
    }
}

# Download SonarQube Scanner
$url = "https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-7.3.0.5189-windows-x64.zip"
$outputFileName = "sonar-scanner.zip"

# Ensure the output directory exists
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
    Write-Host "Created directory: $outputDir" -ForegroundColor Green
}

Write-Host "=== Downloading SonarQube Scanner ===" -ForegroundColor Cyan
$sonarScannerSuccess = Download-File -url $url -outputFileName $outputFileName
if (-not $sonarScannerSuccess) {
    Write-Host "SonarQube Scanner download was cancelled or failed. Continuing with build-wrapper download..." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "=== Downloading Build Wrapper ===" -ForegroundColor Cyan
$buildWrapperUrl = "https://sonarcloud.io/static/cpp/build-wrapper-win-x86.zip"
$buildWrapperFileName = "build-wrapper-win-x86.zip"
$buildWrapperSuccess = Download-File -url $buildWrapperUrl -outputFileName $buildWrapperFileName
if (-not $buildWrapperSuccess) {
    Write-Host "Build Wrapper download was cancelled or failed." -ForegroundColor Yellow
}

Write-Host ""
if ($sonarScannerSuccess -and $buildWrapperSuccess) {
    Write-Host "All downloads completed successfully!" -ForegroundColor Green
} elseif ($sonarScannerSuccess) {
    Write-Host "SonarQube Scanner downloaded successfully. Build Wrapper download was skipped or failed." -ForegroundColor Yellow
} elseif ($buildWrapperSuccess) {
    Write-Host "Build Wrapper downloaded successfully. SonarQube Scanner download was skipped or failed." -ForegroundColor Yellow
} else {
    Write-Host "Both downloads were skipped or failed." -ForegroundColor Yellow
}

