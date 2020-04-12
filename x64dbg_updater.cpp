#include <QtCore>
#include <QtWidgets>
#include <QDataStream>
#include "x64dbg_updater.h"
#include "updateform.h"
#include "settingsform.h"

enum
{
    MENU_CHECKUPDATE,
    MENU_ABOUT,
    MENU_SETTINGS
};

UpdateForm *myDlg;
SettingsForm *settingsDlg;

QString updaterPath;

int pluginHandle;
HWND hwndDlg;
int hMenu;
int hMenuDisasm;
int hMenuDump;
int hMenuStack;

MY_PLUGIN_SETTINGS globalSettings;


void updateAll(bool restart) {
    QString param = "\"" + QDir(globalSettings.managerPath).filePath("x64plgmnrc.exe").append(ArchValue("\" x32 ", "\" x64 ") + QString::number(restart));

    ShellExecuteW(0, L"open", (updaterPath + "_update.bat").toStdWString().c_str(), param.toStdWString().c_str(), updaterPath.toStdWString().c_str(), SW_SHOWDEFAULT);
}


PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
        case MENU_ABOUT: {
            QMessageBox msg(QMessageBox::Information, "About x64dbg Updater v0.2",
                "x64dbg Updater plugin written by:<br>gORDon_vdLg<br>"
                "<br>"
                "Website:<br>"
                "<a href=\"https://github.com/LFriede/x64dbg-updater\">github.com/LFriede/x64dbg-updater</a>"
                "<br><br>"
                "Build:<br>"+
                QString(__DATE__)+
                "<br><hr><br>"
                "x64dbg Plugin Manager written by:<br>Horsicq<br>"
                "<br>"
                "Website:<br>"
                "<a href=\"https://github.com/horsicq/x64dbg-Plugin-Manager\">github.com/horsicq/x64dbg-Plugin-Manager</a>"
            );
            msg.setWindowIcon(QIcon(":/icons/images/information.png"));
            msg.exec();
            break;
        }

        case MENU_SETTINGS: {
            settingsDlg->show();
            break;
        }

        case MENU_CHECKUPDATE: {
            myDlg->checkUpdate();
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
    if (globalSettings.updateOnExit) {
        updateAll(false);
    }

    _plugin_menuclear(hMenu);

    myDlg->close();
    delete myDlg;

    settingsDlg->close();
    delete settingsDlg;

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
    settingsDlg = new SettingsForm();

    BridgeSettingGetUint("UpdaterPlugin", "DelayTimestamp", &globalSettings.updateDelayTimestamp);

    // Show settings dialog to user if there is something to configure first
    duint chk = 1;
    if (
        BridgeSettingGetUint("UpdaterPlugin", "AutocheckOnStartup", &chk) == false ||
        BridgeSettingGet("UpdaterPlugin", "PluginManagerPath", globalSettings.managerPath) == false ||
        BridgeSettingGetUint("UpdaterPlugin", "DelayValue", &globalSettings.updateDelayValue) == false ||
        BridgeSettingGetUint("UpdaterPlugin", "DelayFactor", &globalSettings.updateDelayFactor) == false
    ) {
        globalSettings.autoCheck = chk == 1;
        settingsDlg->show();
    }
    globalSettings.autoCheck = chk == 1;

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

    _plugin_menuaddentry(hMenu, MENU_SETTINGS, "Settings");
    iconf.setFileName(":/icons/images/settings.png");
    if (iconf.open(QIODevice::ReadOnly)) {
        QByteArray arr = iconf.readAll();
        ICONDATA icond;
        icond.data = arr.data();
        icond.size = arr.size();
        _plugin_menuentryseticon(pluginHandle, MENU_SETTINGS, &icond);
        iconf.close();
    }

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
    upath = upath.left(upath.lastIndexOf('\\')+1);
    updaterPath = upath;

    HANDLE hFile;
    hFile = CreateFileW(
        upath.append("commithash.txt").toStdWString().c_str(),
        GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        _plugin_logputs("x64dbg Updater: Error reading commithash.txt");
    }

    char commithash[41];
    DWORD len;
    commithash[0] = 0;
    commithash[40] = 0;
    ReadFile(hFile, &commithash, 40, &len, NULL);
    if (len != 40) {
        CloseHandle(hFile);
        _plugin_logputs("x64dbg Updater: Error reading commithash.txt");
    }
    CloseHandle(hFile);

    myDlg->currentCommitHash = commithash;

    if (globalSettings.autoCheck) {
        QDateTime testTime;
        testTime.setTime_t(globalSettings.updateDelayTimestamp);
        switch (globalSettings.updateDelayFactor) {
            case 0: {
                testTime = testTime.addSecs(globalSettings.updateDelayValue * 60 * 60);
                break;
            }
            case 1: {
                testTime = testTime.addDays(globalSettings.updateDelayValue);
                break;
            }
            case 2: {
                testTime = testTime.addDays(globalSettings.updateDelayValue * 7);
                break;
            }
        }

        if (QDateTime::currentDateTimeUtc() > testTime) {
            myDlg->checkUpdate();
        } else {
            _plugin_logputs((QString("x64dbg Updater: Autocheck delayed until -> ") + testTime.toString(Qt::SystemLocaleShortDate)).toStdString().c_str());
        }
    }
}
