param (
    [string]$InstallDir,
    [string]$AppName,
    [string]$ExePath,
    [string]$IconPath
)

$shortcutPath = "$InstallDir\$AppName.lnk"
$WScriptShell = New-Object -ComObject WScript.Shell
$shortcut = $WScriptShell.CreateShortcut($shortcutPath)
$shortcut.TargetPath = $ExePath
$shortcut.IconLocation = $IconPath
$shortcut.Save()

