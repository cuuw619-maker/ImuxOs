@echo off
powershell -ExecutionPolicy Bypass -File "%~dp0run-qemu.ps1" %*
