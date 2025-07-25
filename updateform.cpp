#include <QMessageBox>
#include <QFile>
#include <QtWidgets>

#include "Windows.h"

#include "x64dbg_updater.h"
#include "updateform.h"
#include "ui_updateform.h"
#include "httpreq.h"

#include "pluginmain.h"

#define EXCEPTION_JSON 1


// Pluginmanager: console_source\main_console.cpp
enum PLGMNGREXITCODE
{
    PLGMNGREXITCODE_NOARGS=0,
    PLGMNGREXITCODE_ERROR,
    PLGMNGREXITCODE_DATAERROR,
    PLGMNGREXITCODE_INSTALLFILES,
    PLGMNGREXITCODE_INSTALLPLUGINS,
    PLGMNGREXITCODE_LISTNAMEISEMPTY,
    PLGMNGREXITCODE_NOINPUTFILES,
    PLGMNGREXITCODE_NOPLUGINSINSTALLED,
    PLGMNGREXITCODE_PLUGINCREATED,
    PLGMNGREXITCODE_CANNOTCREATESERVERLIST,
    PLGMNGREXITCODE_SHOWSERVERLIST,
    PLGMNGREXITCODE_SERVERLISTCREATED,
    PLGMNGREXITCODE_SERVERLISTISEMPTY,
    PLGMNGREXITCODE_SERVERLISTUPDATED,
    PLGMNGREXITCODE_SHOWINSTALLED,
    PLGMNGREXITCODE_SHOWUPDATES,
    PLGMNGREXITCODE_UPDATEALL,
    PLGMNGREXITCODE_UPDATEPLUGINS,
    PLGMNGREXITCODE_REMOVEPLUGINS,
    PLGMNGREXITCODE_NOUPDATESAVAILABLE,
};


UpdateForm::UpdateForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateForm)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Default/icons/update.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->trwCommits->setHeaderLabel("Latest commits");

    QPalette p = ui->pteUpdaterConsole->palette();
    p.setColor(QPalette::Base, QColor(0, 0, 0));
    p.setColor(QPalette::Text, QColor(0xCC, 0xCC, 0xCC));
    ui->pteUpdaterConsole->setPalette(p);

    // Setting the font(family) in the designer makes us crash after plugin unload O_o
    QFont font;
    font.setFamily("Consolas");
    font.setPointSize(10);
    ui->pteUpdaterConsole->setFont(font);

    foundCommitDate = false;

    ui->pbUpdateOnExit->setChecked(globalSettings.updateOnExit);

    updaterProcess = new QProcess(this);
    connect(updaterProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(updaterProcessError(QProcess::ProcessError)));
}

void UpdateForm::checkUpdate() {
    // Save the date ;)
    globalSettings.updateDelayTimestamp = QDateTime::currentDateTimeUtc().toTime_t();
    BridgeSettingSetUint("UpdaterPlugin", "DelayTimestamp", globalSettings.updateDelayTimestamp);

    // Let PluginManager update the server list
    disconnect(updaterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
    connect(updaterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(plgmgrUpdateServerListFinished(int, QProcess::ExitStatus)));
    updaterProcess->start(("\"" + QDir(globalSettings.managerPath).filePath("x64plgmnrc.exe").append("\" --updateserverlist")).toStdString().c_str());

    // Get list of commits from GitHub
    HttpReq *request = new HttpReq(this, "https://api.github.com/repos/x64dbg/x64dbg/commits");
    connect(request, &HttpReq::resultReady, this, &UpdateForm::replyFinished_commits);
    connect(request, &HttpReq::finished, request, &QObject::deleteLater); // autofree thread
    request->start();
}

void UpdateForm::plgmgrUpdateServerListFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    // Server list is updated, check for updates
    disconnect(updaterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
    connect(updaterProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(plgmgrShowUpdatesFinished(int, QProcess::ExitStatus)));
    updaterProcess->start(("\"" + QDir(globalSettings.managerPath).filePath("x64plgmnrc.exe").append("\" --showupdates")).toStdString().c_str());
}

void UpdateForm::plgmgrShowUpdatesFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    QString s = updaterProcess->readAllStandardOutput();

    if (exitCode == PLGMNGREXITCODE_SHOWUPDATES) {
        ui->lblProgress->setText("Status: PluginManager found new updates.");
        _plugin_logputs("x64dbg Updater: PluginManager found new updates.");
        show();
    } else {
        ui->lblProgress->setText("Status: PluginManager found no new updates.");
        _plugin_logputs("x64dbg Updater: PluginManager found no new updates.");
    }

    s = "> x64plgmnrc.exe --showupdates\x0d\x0a\x0d\x0a" + s;
    ui->pteUpdaterConsole->setPlainText(s);
}


void UpdateForm::replyFinished_commits(const unsigned long status, const QString content) {
    // content contains error if status is not 200
    if (status != 200) {
        _plugin_logputs((QString("x64dbg Updater error: ") + content).toLocal8Bit().data());
        return;
    }

    json_t *json = NULL;

    ui->trwCommits->clear();
    char theme[MAX_SETTING_SIZE];
    bool darkMode = false;
    if (BridgeSettingGet("Theme", "Selected", theme) == true) {
        if (strcmp(theme, "Dark") == 0) {
            darkMode = true;
        }
    }

    try {
        json_error_t error;
        json = json_loads(content.toUtf8(), 0, &error);
        if (!json) { throw EXCEPTION_JSON; }

        if (!json_is_array(json)) { throw EXCEPTION_JSON; }

        QDate lastDate = QDate();
        QString latestCommitHash;
        QTreeWidgetItem *treeRoot = nullptr;
        bool alternatingFlag = false;
        for (size_t i = 0; i < json_array_size(json); i++) {
            json_t *commit;
            commit = json_array_get(json, i);
            if (!json_is_object(commit)) { throw EXCEPTION_JSON; }

            json_t *commithash;
            commithash = json_object_get(commit, "sha");
            if (!json_is_string(commithash)) { throw EXCEPTION_JSON; }
            QString sCommitHash = json_string_value(commithash);


            json_t *commit_;
            commit_ = json_object_get(commit, "commit");
            if (!json_is_object(commit_)) { throw EXCEPTION_JSON; }

            json_t *message;
            message = json_object_get(commit_, "message");
            if (!json_is_string(message)) { throw EXCEPTION_JSON; }
            QString sMessage = json_string_value(message);

            json_t *committer;
            committer = json_object_get(commit_, "committer");
            if (!json_is_object(committer)) { throw EXCEPTION_JSON; }

            json_t *date;
            date = json_object_get(committer, "date");
            if (!json_is_string(date)) { throw EXCEPTION_JSON; }
            QDateTime datetime = QDateTime::fromString(json_string_value(date), Qt::ISODate);

            if (lastDate != datetime.date()) {
                lastDate = datetime.date();
                treeRoot = new QTreeWidgetItem(ui->trwCommits);
                treeRoot->setText(0, lastDate.toString());
                treeRoot->setExpanded(true);
            }

            QTreeWidgetItem *treeItem = new QTreeWidgetItem();
            treeItem->setText(0, sMessage);
            if (sCommitHash == currentCommitHash) {
                treeItem->setBackgroundColor(0, QColor(255, 128, 64));
                ui->lblCurrentVersion->setText("Current commit:\t" + currentCommitHash.left(7) + " - " + datetime.toLocalTime().toString());
                foundCommitDate = true;
            } else {
                if (alternatingFlag) {
                    if (darkMode) {
                        treeItem->setBackgroundColor(0, QColor(137, 162, 246));
                    } else {
                        treeItem->setBackgroundColor(0, QColor(153, 217, 234));
                    }
                } else {
                    if (darkMode) {
                        treeItem->setBackgroundColor(0, QColor(132, 107, 244));
                    } else {
                        treeItem->setBackgroundColor(0, QColor(213, 240, 247));
                    }
                }
                alternatingFlag = !alternatingFlag;
            }
            treeRoot->addChild(treeItem);

            if (i == 0) {
                latestCommitHash = sCommitHash;
                ui->lblLatestVersion->setText("Latest commit:\t" + sCommitHash.left(7) + " - " + datetime.toLocalTime().toString());
            }
        }

        json_decref(json);
    } catch (int e) {
        switch (e) {
            case EXCEPTION_JSON: {
                _plugin_logputs("x64dbg Updater: JSON error");
                break;
            }
        }

        if (json) {
            json_decref(json);
        }
    }
}

void UpdateForm::showEvent(QShowEvent *event) {
    if (!foundCommitDate) {
        ui->lblCurrentVersion->setText("Current commit:\t" + currentCommitHash.left(7));
    }
}

void UpdateForm::updaterProcessError(QProcess::ProcessError error) {
    _plugin_logputs("x64dbg-updater error: Faild to start x64plgmnrc.exe");
    ui->pteUpdaterConsole->setPlainText("Faild to start x64plgmnrc.exe");
}

UpdateForm::~UpdateForm() {
    delete ui;
    delete updaterProcess;
}

void UpdateForm::on_pbShowPluginManager_clicked()
{
    updateOnExit(false);
    HINSTANCE res = ShellExecuteW(0, L"open", QDir(globalSettings.managerPath).filePath("x64plgmnr.exe").toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
    if (res <= (HINSTANCE)32) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Icon::Critical);
        msgBox.setWindowIcon(msgBox.style()->standardIcon(QStyle::SP_MessageBoxCritical));
        msgBox.setText("Unable to start x64plgmnr.exe, check path in settings.");
        msgBox.exec();
    } else {
        close();
    }
}

void UpdateForm::on_pbUpdateAll_clicked()
{
    updateOnExit(false);
    updateAll(true);

    PostMessage(hwndDlg, WM_CLOSE, 0, 0);
    close();
}

void UpdateForm::updateOnExit(bool enabled) {
    globalSettings.updateOnExit = enabled;
    ui->pbUpdateOnExit->setChecked(enabled);
}

void UpdateForm::on_pbUpdateOnExit_clicked()
{
    globalSettings.updateOnExit = !globalSettings.updateOnExit;
}

void UpdateForm::on_pbForceCoreUpdate_clicked()
{
    updateOnExit(false);
    forceUpdate();

    PostMessage(hwndDlg, WM_CLOSE, 0, 0);
    close();
}
