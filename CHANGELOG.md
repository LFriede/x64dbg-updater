# x64dbg Updater Changelog


## 0.4

### tl;dr

 - Mostly cosmetic sh... stuff, but this version works again with the latest x64dbg releases!

### Details
 - Fixed plugin to work with x64dbg version **2025.06.30**.
   - Binarys are now built using Qt 5.12.12 and VS2022 with 2017 runtime.
   - Commit list is fetched by WinINet API instead of Qt because Qt5Network component is not shipped by x64dbg anymore.
 - Removed `_update.bat`, commands will be passed directly to `cmd.exe`.
 - Added option to force x64dbg core package update/reinstall.
 - Better colors for dark theme.
 - The `Update all on exit` button can now be toggled on and off. Previously the `Do nothing` button was there to reset the flag, but that was misleading crappy design.
 - Menu and window icons are shown again. This was broken since a x64dbg build a few years ago.
 - GPLv3 license is now included.


## 0.3

 - Now checks for updates by invoking the [Plugin Manager](https://github.com/horsicq/x64dbg-Plugin-Manager) by Horsicq.
 - Implemented the ability to configure a timeout between automatic update checks.
 - You can now choose to install updates when you close x64dbg. So you can start to work without waiting and still get the updates.


## 0.2

 - Now compares commit ID instead of date to find new versions.
 - Shows last commits in GUI.
 - Minor tweaks.
