#ifndef X64DBG_UPDATER_GLOBAL_H
#define X64DBG_UPDATER_GLOBAL_H

#include <QtCore/qglobal.h>

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

#endif // X64DBG_UPDATER_GLOBAL_H
