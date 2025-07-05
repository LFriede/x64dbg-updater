@echo off

call build_version.bat

set pluginroot=release\pluginroot\

mkdir %pluginroot%x32\plugins
mkdir %pluginroot%x64\plugins

call:copyfile release\x64dbg_updater.dp32 %pluginroot%x32\plugins\
call:copyfile release\x64dbg_updater.dp64 %pluginroot%x64\plugins\
call:copyfile README.md %pluginroot%README_updater.md
call:copyfile CHANGELOG.md %pluginroot%CHANGELOG_updater.md
call:copyfile LICENSE %pluginroot%LICENSE_updater

set lastdir=%cd%
cd %pluginroot%
7za a %lastdir%\release\x64dbg-updater-bin-%MY_BUILD_VERSION%.zip *
cd %lastdir%

pause
exit

REM functions

:copyfile
    xcopy /Y /-I %~1 %~2

    if not %ERRORLEVEL%==0 (
        color 0c
        echo An error occured. Aborting.
        pause
        color 07
        exit
    )
goto:eof
