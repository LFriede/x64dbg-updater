#include <QtCore>
#include <QtWidgets>
#include <QDataStream>
#include "x64dbg_updater.h"
#include "updateform.h"

enum
{
    MENU_CHECKUPDATE,
    MENU_ABOUT,
    MENU_AUTOCHECK
};

UpdateForm *myDlg;

int pluginHandle;
HWND hwndDlg;
int hMenu;
int hMenuDisasm;
int hMenuDump;
int hMenuStack;

bool autoCheck = false;

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
        case MENU_ABOUT: {
            QMessageBox msg(QMessageBox::Information, "About x64dbg Updater v0.2", "Plugin written by:<br>gORDon_vdLg<br><br>Website:<br><a href=\"https://github.com/LFriede/x64dbg-updater\">github.com/LFriede/x64dbg-updater</a><br><br>Build:<br>"+QString(__DATE__));
            msg.setWindowIcon(QIcon(":/icons/images/information.png"));
            msg.exec();
            break;
        }

        case MENU_AUTOCHECK: {
            autoCheck = !autoCheck;
            _plugin_menuentrysetchecked(pluginHandle, MENU_AUTOCHECK, autoCheck);
            BridgeSettingSetUint("UpdaterPlugin", "AutocheckOnStartup", autoCheck);
            break;
        }

        case MENU_CHECKUPDATE: {
            myDlg->show();
            break;
        }

        default: {
            break;
        }
    }
}

PLUG_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    initStruct->pluginVersion = PLUGIN_VERSION;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    strncpy_s(initStruct->pluginName, PLUGIN_NAME, _TRUNCATE);
    pluginHandle = initStruct->pluginHandle;

    return true;
}

PLUG_EXPORT bool plugstop()
{
    _plugin_menuclear(hMenu);
    myDlg->close();
    delete myDlg;
    return true;
}

PLUG_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    hwndDlg = setupStruct->hwndDlg;
    hMenu = setupStruct->hMenu;
    hMenuDisasm = setupStruct->hMenuDisasm;
    hMenuDump = setupStruct->hMenuDump;
    hMenuStack = setupStruct->hMenuStack;

    myDlg = new UpdateForm();

    duint chk = 0;
    if (BridgeSettingGetUint("UpdaterPlugin", "AutocheckOnStartup", &chk) == false) {
        QMessageBox msg(QMessageBox::Question, "Autocheck?", "Should x64dbg Updater check for updates on every launch?", QMessageBox::Yes|QMessageBox::No);
        msg.setWindowIcon(QIcon(":/icons/images/question.png"));
        int reply = msg.exec();
        chk = reply == QMessageBox::Yes;
        BridgeSettingSetUint("UpdaterPlugin", "AutocheckOnStartup", chk);
    }
    autoCheck = chk == 1;

    _plugin_menuaddentry(hMenu, MENU_CHECKUPDATE, "Search updates...");
    QFile iconf(":/icons/images/update.png");
    if (iconf.open(QIODevice::ReadOnly)) {
        QByteArray arr = iconf.readAll();
        ICONDATA icond;
        icond.data = arr.data();
        icond.size = arr.size();
        _plugin_menuseticon(hMenu, &icond);
        _plugin_menuentryseticon(pluginHandle, MENU_CHECKUPDATE, &icond);
        iconf.close();
    }

    _plugin_menuaddentry(hMenu, MENU_AUTOCHECK, "Autocheck on startup");
    _plugin_menuentrysetchecked(pluginHandle, MENU_AUTOCHECK, autoCheck);

    _plugin_menuaddentry(hMenu, MENU_ABOUT, "About...");
    iconf.setFileName(":/icons/images/information.png");
    if (iconf.open(QIODevice::ReadOnly)) {
        QByteArray arr = iconf.readAll();
        ICONDATA icond;
        icond.data = arr.data();
        icond.size = arr.size();
        _plugin_menuentryseticon(pluginHandle, MENU_ABOUT, &icond);
        iconf.close();
    }


    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(NULL, path, MAX_PATH)) { return; }

    QString upath = QString::fromWCharArray(path);
    upath = upath.left(upath.lastIndexOf('\\'));
    upath = upath.left(upath.lastIndexOf('\\'));
    upath = upath.left(upath.lastIndexOf('\\')+1);
    myDlg->updaterPath = upath;

    HANDLE hFile;
    hFile = CreateFileW(
        upath.append("commithash.txt").toStdWString().c_str(),
        GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        _plugin_logputs("x64dbg Updater: Error reading commithash.txt");
        return;
    }

    char commithash[41];
    DWORD len;
    commithash[40] = 0;
    ReadFile(hFile, &commithash, 40, &len, NULL);
    if (len != 40) {
        CloseHandle(hFile);
        _plugin_logputs("x64dbg Updater: Error reading commithash.txt");
        return;
    }
    CloseHandle(hFile);

    myDlg->currentCommitHash = commithash;


    if (autoCheck) {
        myDlg->checkUpdate(true);
    }
}
