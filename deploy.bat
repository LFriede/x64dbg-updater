@echo off

rem get current date and time to %fullstamp%
rem https://stackoverflow.com/questions/19131029/how-to-get-date-in-bat-file/19131662#19131662

for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"

set "fullstamp=%YYYY%-%MM%-%DD%_%HH%-%Min%"
set "binname=binaries_%fullstamp%.zip"
set "dbgname=pdb_symbols_%fullstamp%.zip"

rem pack and rename in archive
_res\7za.exe a -tzip %binname% .\_res\7za.exe .\_res\License_7zip.txt .\release\x64dbg_updater.dp32 .\release\x64dbg_updater.dp64 README.md _update.bat
_res\7za.exe rn %binname% x64dbg_updater.dp32 release\x32\plugins\x64dbg_updater.dp32 x64dbg_updater.dp64 release\x64\plugins\x64dbg_updater.dp64 README.md README_updater.md

_res\7za.exe a -tzip %dbgname% .\release\x64dbg_updater.dp32.pdb .\release\x64dbg_updater.dp64.pdb README.md
_res\7za.exe rn %dbgname% x64dbg_updater.dp32.pdb release\x32\plugins\x64dbg_updater.dp32.pdb x64dbg_updater.dp64.pdb release\x64\plugins\x64dbg_updater.dp64.pdb README.md README_updater.md
pause
