param(
  [string]$Iso = "$(Join-Path $PSScriptRoot '..\build\ImuxOS.iso')",
  [int]$Memory = 512
)
$ErrorActionPreference = "Stop"
$qemu = Get-Command qemu-system-x86_64.exe -ErrorAction SilentlyContinue
if (-not $qemu) { throw "qemu-system-x86_64.exe was not found in PATH. Install QEMU for Windows first." }
$isoPath = (Resolve-Path $Iso -ErrorAction Stop).Path
Write-Host "Starting ImuxOS from: $isoPath"
& $qemu.Source -accel tcg -m "${Memory}M" -cdrom $isoPath -boot d
