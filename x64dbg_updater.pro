#-------------------------------------------------
#
# Project created by QtCreator 2017-12-03T15:59:56
#
#-------------------------------------------------

QT       += core widgets network

TARGET = x64dbg_updater
TEMPLATE = lib

DEFINES += X64DBG_UPDATER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    settingsform.cpp \
        x64dbg_updater.cpp \
    updateform.cpp

HEADERS += \
        settingsform.h \
        x64dbg_updater.h \
        x64dbg_updater_global.h \ 
    pluginmain.h \
    updateform.h

win32 {
    DEFINES += "WINVER=0x0500"

    contains(QT_ARCH, i386) {
        QMAKE_POST_LINK += move $$PWD\release\x64dbg_updater.dll $$PWD\release\x64dbg_updater.dp32
        QMAKE_LFLAGS += /PDB:$$PWD\release\x64dbg_updater.dp32.pdb
    } else {
        QMAKE_POST_LINK += move $$PWD\release\x64dbg_updater.dll $$PWD\release\x64dbg_updater.dp64
        QMAKE_LFLAGS += /PDB:$$PWD\release\x64dbg_updater.dp64.pdb
    }
}

# for debug purposes create pdb file for release build
QMAKE_CXXFLAGS += /Zi
QMAKE_LFLAGS += /INCREMENTAL:NO /DEBUG /Profile
# by specifying OBJECTS_DIR we don't have to rebuild all if we change arch (with "shadow build" disabled)
OBJECTS_DIR = out_$$QT_ARCH
# with this line pluginsdk libs will be found with or without "shadow build" enabled
LIBS += -L"$$PWD"
# this makes sure that "deploy.bat" finds the binarys if "shadow build" is enabled (qt default)
DESTDIR = "$$PWD/release"


FORMS += \
    settingsform.ui \
    updateform.ui

DISTFILES += \
    CHANGELOG.md \
    _update.bat \
    .gitignore \
    README.md \
    deploy.bat
