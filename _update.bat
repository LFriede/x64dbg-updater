@echo off

%1 --updateall

if "%3" neq "0" (
    start "" "%2\%2dbg.exe"
)

pause
