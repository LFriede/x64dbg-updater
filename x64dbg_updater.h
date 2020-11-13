#ifndef X64DBG_UPDATER_H
#define X64DBG_UPDATER_H

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")

#include "x64dbg_updater_global.h"
#include "pluginmain.h"

#define PLUGIN_NAME "x64dbg Updater"
#define PLUGIN_VERSION 1

void updateAll(bool restart);
void forceUpdate();

#endif // X64DBG_UPDATER_H
