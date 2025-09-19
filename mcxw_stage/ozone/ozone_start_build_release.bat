@echo off
cd /D %~dp0
start "" "c:\program files\segger\ozone\ozone.exe" -project .\mcxw_stage_build_release.jdebug
