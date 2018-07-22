:start
@echo off
color 0c
echo.
echo WARNING!
echo This script will delete EVERY file and directory below this root directory
echo except a few whitelisted ones like x64dbg config, plugins and databases.
echo Please review this batch file for yourself and edit it suiting your personal
echo needs.
echo.
echo Please close all running x64dbg processes and backup your data!
echo.
echo If you want to delete stuff type "stfu" to continue.

set /P _stfu=
if "%_stfu%" neq "stfu" goto :start
color 07
echo on


for %%i in (*) do call :removefile "%%i"
for /D %%i in (*) do call :removedir "%%i"

for %%i in (release\*) do call :removefile "%%i"
for /D %%i in (release\*) do call :removedir "%%i"

for %%i in (release\x32\*) do call :removefile "%%i"
for /D %%i in (release\x32\*) do call :removedir "%%i"

for %%i in (release\x64\*) do call :removefile "%%i"
for /D %%i in (release\x64\*) do call :removedir "%%i"

7za.exe x %1

del /F %1

goto :end


:removefile
  @rem whitelist
  if %1 == "_update.bat" goto :eof
  if %1 == "7za.exe" goto :eof
  if %1 == "License_7zip.txt" goto :eof

  if %1 == "release\x32\x32dbg.ini" goto :eof
  if %1 == "release\x64\x64dbg.ini" goto :eof
  if %1 == "release\x32\symsrv.yes" goto :eof
  if %1 == "release\x64\symsrv.yes" goto :eof
  
  del /F %1
goto :eof


:removedir
  @rem whitelist
  if %1 == "release\x32\db" goto :eof
  if %1 == "release\x64\db" goto :eof
  if %1 == "release\x32\plugins" goto :eof
  if %1 == "release\x64\plugins" goto :eof
  if %1 == "release\x32" goto :eof
  if %1 == "release\x64" goto :eof
  if %1 == "release" goto :eof
  
  rd /S /Q %1
goto :eof


:end
@echo off
echo Hit ENTER to restart %2dbg, close if you don't want to.
pause > nul
start "" "release\%2\%2dbg.exe"
