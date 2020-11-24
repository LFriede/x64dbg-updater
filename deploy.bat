@echo off

call build_version.bat


mkdir release\pluginroot\x32\plugins
mkdir release\pluginroot\x64\plugins

copy release\x64dbg_updater.dp32 release\pluginroot\x32\plugins\
copy release\x64dbg_updater.dp64 release\pluginroot\x64\plugins\
copy README.md release\pluginroot\README_updater.md

x64plgmnrc.exe --createplugin x64dbg_updater --setrootpath release\pluginroot --setname x64dbg_updater --setversion %MY_BUILD_VERSION% --setauthor gORDon_vdLg --setbugreport "https://github.com/LFriede/x64dbg-updater/issues" --setinfo "Automatic update checks with the plugin manager on x64dbg startup."

pause
