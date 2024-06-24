param (
    [string]$InstallDir,
    [string]$AppName,
    [string]$ExePath
)

$shortcutPath = "$InstallDir\$AppName.lnk"
$WScriptShell = New-Object -ComObject WScript.Shell
$shortcut = $WScriptShell.CreateShortcut($shortcutPath)
$shortcut.TargetPath = $ExePath
$shortcut.Save()

