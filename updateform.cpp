#include <QMessageBox>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QtWidgets>

#include "Windows.h"

#include "x64dbg_updater_global.h"
#include "updateform.h"
#include "ui_updateform.h"

#include "pluginmain.h"

#define EXCEPTION_NETWORK 0
#define EXCEPTION_JSON 1

UpdateForm::UpdateForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateForm)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/images/update.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->trwCommits->setHeaderLabel("Latest commits");

    autoUpdateFlag = false;
    foundCommitDate = false;

    manager = new QNetworkAccessManager(this);
}

void UpdateForm::checkUpdate(bool autoUpdate) {
    autoUpdateFlag = autoUpdate;

    disconnect(manager, SIGNAL(finished(QNetworkReply*)), 0, 0);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished_commits(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://api.github.com/repos/x64dbg/x64dbg/commits")));
}

void UpdateForm::downloadFinished(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError) { //error
        _plugin_logprint("x64dbg Updater: network error -> ");
        _plugin_logputs(reply->errorString().toStdString().c_str());
        reply->deleteLater();
        return;
    }

    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 302) { // http redirect
        dlFile.reset(); // flush file because of redirect message
        dlReply = manager->get(QNetworkRequest(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl()));
        connect(dlReply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
        connect(dlReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
        reply->deleteLater();
        return;
    }

    dlFile.close();

    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->lblProgress->setText("Status: Download finished, updater triggered.");

    QMessageBox msgBox;
    msgBox.setText("Updater script will be launched, please close all x64dbg instances before continuing the updater script.");
    msgBox.setWindowIcon(QIcon(":/icons/images/information.png"));
    QAbstractButton* pButtonClose = msgBox.addButton("Close x64dbg now!", QMessageBox::YesRole);
    msgBox.addButton("Close later...", QMessageBox::NoRole);
    msgBox.exec();

    QString param = "\"" + dlFile.fileName().replace("/", "\\").append(ArchValue("\" x32", "\" x64"));
    ShellExecuteW(0, L"open", updaterPath.append("_update.bat").toStdWString().c_str(), param.toStdWString().c_str(), updaterPath.toStdWString().c_str(), SW_SHOWDEFAULT);

    if (msgBox.clickedButton() == pButtonClose) {
        PostMessage(hwndDlg, WM_CLOSE, 0, 0);
    }

    hide();

    reply->deleteLater();
}

void UpdateForm::downloadReadyRead() {
    int progress = ui->progressBar->value() + dlReply->size();
    ui->progressBar->setValue(progress);
    ui->lblProgress->setText("Status: " + QString().setNum(progress / 1024.0 / 1024.0, 'f' ,2) + " MiB of " + QString().setNum(dlSize / 1024.0 / 1024.0, 'f' ,2) + " MiB downloaded.");
    dlFile.write(dlReply->readAll());
}

void UpdateForm::replyFinished_commits(QNetworkReply *reply) {
    reply->deleteLater();

    json_t *json = NULL;

    ui->trwCommits->clear();

    try {
        if(reply->error() != QNetworkReply::NoError) {
            throw EXCEPTION_NETWORK;
        }

        json_error_t error;
        json = json_loads(reply->readAll().data(), 0, &error);
        reply->deleteLater();
        if (!json) { throw EXCEPTION_JSON; }

        if (!json_is_array(json)) { throw EXCEPTION_JSON; }

        QDate lastDate = QDate();
        QString latestCommitHash;
        QTreeWidgetItem *treeRoot = nullptr;
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

            json_t *author;
            author = json_object_get(commit_, "author");
            if (!json_is_object(author)) { throw EXCEPTION_JSON; }

            json_t *date;
            date = json_object_get(author, "date");
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
                treeItem->setBackgroundColor(0, QColor(153, 217, 234));
                ui->lblCurrentVersion->setText("Current commit:\t" + currentCommitHash.left(7) + " - " + datetime.toLocalTime().toString());
                foundCommitDate = true;
            }
            treeRoot->addChild(treeItem);

            if (i == 0) {
                latestCommitHash = sCommitHash;
                ui->lblLatestVersion->setText("Latest commit:\t" + sCommitHash.left(7) + " - " + datetime.toLocalTime().toString());
            }
        }

        json_decref(json);


        if (autoUpdateFlag) {
            if (latestCommitHash != currentCommitHash) {
                _plugin_logputs("x64dbg Updater: autoCheck found new commits.");
                show();
            } else {
                _plugin_logputs("x64dbg Updater: autoCheck found no new commits.");
            }
        }
        autoUpdateFlag = false;

    } catch (int e) {
        switch (e) {
            case EXCEPTION_NETWORK: {
                _plugin_logprint("x64dbg Updater: network error -> ");
                _plugin_logputs(reply->errorString().toStdString().c_str());
                break;
            }
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

void UpdateForm::replyFinished_releases(QNetworkReply *reply) {
    reply->deleteLater();

    json_t *json = NULL;

    try {
        if(reply->error() != QNetworkReply::NoError) { //error
            throw EXCEPTION_NETWORK;
        }

        json_error_t error;
        char* x = reply->readAll().data();
        json = json_loads(x, 0, &error);
        if (!json) { throw EXCEPTION_JSON; }

        if (!json_is_object(json)) { throw EXCEPTION_JSON; }


        json_t *url;
        json_t *assets;
        assets = json_object_get(json, "assets");
        if (!json_is_array(assets)) { throw EXCEPTION_JSON; }
        assets = json_array_get(assets, 0);
        if (!json_is_object(assets)) { throw EXCEPTION_JSON; }

        url = json_object_get(assets, "browser_download_url");
        if (!json_is_string(url)) { throw EXCEPTION_JSON; }
        updateUrl = json_string_value(url);


        json_t *size = json_object_get(assets, "size");
        if (!json_is_integer(size)) { throw EXCEPTION_JSON; }

        dlSize = json_integer_value(size);
        ui->progressBar->setMaximum(dlSize);
        ui->lblProgress->setText("Status: "+QString().setNum(dlSize / 1024.0 / 1024.0, 'f' ,2)+" MiB to download");

        json_decref(json);

        ui->progressBar->setEnabled(true);

    } catch (int e) {
        switch (e) {
            case EXCEPTION_NETWORK: {
                _plugin_logprint("x64dbg Updater: network error -> ");
                _plugin_logputs(reply->errorString().toStdString().c_str());
                break;
            }
            case EXCEPTION_JSON: {
                _plugin_logputs("x64dbg Updater: JSON error");
                break;
            }
        }

        if (json) {
            json_decref(json);
        }
    }


    // Download file
    TCHAR path[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, path)) { return; }

    disconnect(manager, SIGNAL(finished(QNetworkReply*)), 0, 0);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

    dlFile.setFileName(QString::fromWCharArray(path).append("x64dbg_snapshot.zip"));
    if (!dlFile.open(QIODevice::ReadWrite)) { return; }

    dlReply = manager->get(QNetworkRequest(QUrl(updateUrl)));
    connect(dlReply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
}

void UpdateForm::showEvent(QShowEvent *event) {
    if (!foundCommitDate) {
        ui->lblCurrentVersion->setText("Current commit:\t" + currentCommitHash.left(7));
    }

    if (autoUpdateFlag == false) {
        checkUpdate(false);
    }
}

UpdateForm::~UpdateForm() {
    delete ui;
    delete manager;
}

void UpdateForm::on_applyButton_clicked() {
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), 0, 0);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished_releases(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://api.github.com/repos/x64dbg/x64dbg/releases/latest")));
}
