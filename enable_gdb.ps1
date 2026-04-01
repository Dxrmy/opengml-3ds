$iniPath = "$env:APPDATA\Citra\config\sdl2-config.ini"
$content = Get-Content $iniPath

$modified = $false
$newContent = @()

foreach ($line in $content) {
    if ($line -match "^Debugging_UseGdbstub\s*=") {
        $newContent += "Debugging_UseGdbstub = true"
        $modified = $true
    } elseif ($line -match "^Debugging_GdbstubPort\s*=") {
        $newContent += "Debugging_GdbstubPort = 24689"
    } else {
        $newContent += $line
    }
}

if (-not $modified) {
    Write-Host "Warning: Could not find Debugging_UseGdbstub in ini file. You may need to add it under [Debugging]."
}

Set-Content -Path $iniPath -Value $newContent
Write-Host "Enabled GDB in Citra config"