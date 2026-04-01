$logPath = "$env:APPDATA\Citra\log\citra_log.txt"
Get-Content -Path $logPath -Wait -Tail 20 | ForEach-Object {
    if ($_ -match "FATAL" -or $_ -match "CRITICAL") {
        Write-Host $_ -ForegroundColor Red
    }
    elseif ($_ -match "DataWinLoader") {
        Write-Host $_ -ForegroundColor Cyan
    }
    elseif ($_ -match "OpenGML") {
        Write-Host $_ -ForegroundColor Yellow
    }
    else {
        Write-Host $_
    }
}