# x64dbg Updater

This x64dbg plugin checks GitHub for new x64dbg snapshots. It can download and extract the new snapshot to replace your current version.  
At the first start with this plugin installed you will be asked if you want to autocheck for updates on every launch. This setting can be changed later in the plugins menu of x64dbg. There is also a menu entry for manual update checking to be used if you don't want to autocheck.

![Screenshot](_res/screenshot.png?raw=true)


## Installation

Binary versions can be downloaded on the GitHub [release page](https://github.com/LFriede/x64dbg-updater/releases). The directory structure of the releases matches with the x64dbg snapshots, this makes installation easy, just unpack x64dbg and the updater in the same directory. Please **don't** relocate the `_updater.bat` script.  
`*.pdb` can be downloaded in a seperate archive on the release page and should be unpacked in the same directory as the plugin file.


## Build

Make sure you have Qt Creator (current version) with Qt 5.6.x and Visual Studio (2015 in my case, others should work) installed. Optional you can add syntax highlighting for the batch script in the settings dialog of Qt Creator.  
If you switch between x64 and x86 you may have to hit "rebuild all" in Qt Creator instead of just hitting "build".  
Make sure you build in release mode because .lib files of the plugin SDK are built in release mode (must match) and x64dbg is shipped with release binarys of Qt. The Qt project file is configured to generate debug symbols (pdb) in release mode too, place them in x64dbgs plugin folder if you want to debug the plugin.
