#ifndef X64DBG_UPDATER_GLOBAL_H
#define X64DBG_UPDATER_GLOBAL_H

#include <QtCore/qglobal.h>
#include "pluginmain.h"

#if defined(X64DBG_UPDATER_LIBRARY)
#  define X64DBG_UPDATERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define X64DBG_UPDATERSHARED_EXPORT Q_DECL_IMPORT
#endif

#ifdef _WIN64
#define ArchValue(x32value, x64value) x64value
#else
#define ArchValue(x32value, x64value) x32value
#endif //_WIN64


struct MY_PLUGIN_SETTINGS {
    bool autoCheck = false;
    char managerPath[MAX_SETTING_SIZE];
    bool updateOnExit = false;
    duint updateDelayValue = 1;
    duint updateDelayFactor = 1;          // 0 = hours, 1 = days, 2 = weeks
    duint updateDelayTimestamp = 0;
};
extern MY_PLUGIN_SETTINGS globalSettings;

#endif // X64DBG_UPDATER_GLOBAL_H
