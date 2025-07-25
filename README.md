# x64dbg Updater

This x64dbg plugin invokes the [x64dbg Plugin Manager](https://github.com/horsicq/x64dbg-Plugin-Manager) to check for x64dbg or plugin updates. You can configure the plugin to check for updates at x64dbg startup or you can click the "Search updates..." button in the menu. It's also possible to configure a pause between automatic update checks, so you can let the plugin check for example daily or weekly.

You can launch the [x64dbg Plugin Manager](https://github.com/horsicq/x64dbg-Plugin-Manager) from the gui, update everything with one click or let it update when you close x64dbg.

There is also a widget that shows you the latest x64dbg commits from GitHub. The latest commit of your installed version is highlighted in orange, so you can see what's new.

![Screenshot](_res/screenshot.png?raw=true)


## Installation

Since this plugin depends on the [x64dbg Plugin Manager](https://github.com/horsicq/x64dbg-Plugin-Manager), the easiest way to install it is to install the plugin manager and let it download the x64dbg_updater package.

On the first launch with this plugin installed the configuration dialog pops up. It's important to enter the path to the `base` directory of the Plugin Manager because the updater needs to find `x64plgmnrc.exe`.


## Build

To build the plugin every Qt Creator, Qt 5.x and Visual Studio version should work. Released binarys are always built with the same environment x64dbg is built with. At time of writing this is Qt 5.12.12, VS2022 with 2017 C++ runtimes.

Make sure you build in release mode because .lib files of the plugin SDK are built in release mode (must match) and x64dbg is shipped with release binarys of Qt. The Qt project file is configured to generate debug symbols (pdb) in release mode too.
