#include <QMessageBox>
#include <QtNetwork/QNetworkReply>
#include <QFile>

#include "Windows.h"

#include "x64dbg_updater_global.h"
#include "updateform.h"
#include "ui_updateform.h"

#include "pluginmain.h"

UpdateForm::UpdateForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateForm)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/images/update.png"));
    setWindowFlags(Qt::Dialog);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    autoUpdateFlag = false;

    manager = new QNetworkAccessManager(this);
}

void UpdateForm::checkUpdate(bool autoUpdate) {
    autoUpdateFlag = autoUpdate;

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl("https://api.github.com/repos/x64dbg/x64dbg/releases/latest")));
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

void UpdateForm::replyFinished(QNetworkReply *reply) {
    if(reply->error() != QNetworkReply::NoError) { //error
        _plugin_logprint("x64dbg Updater: network error -> ");
        _plugin_logputs(reply->errorString().toStdString().c_str());
        reply->deleteLater();
        return;
    }

    json_error_t error;
    json_t *json = json_loads(reply->readAll().data(), 0, &error);
    reply->deleteLater();
    if (!json) { return; }

    if (!json_is_object(json)) {
        json_decref(json);
        return;
    }

    json_t *date;
    date = json_object_get(json, "published_at");
    if (!json_is_string(date)) {
        json_decref(json);
        return;
    }

    QDateTime datetime = QDateTime::fromString(json_string_value(date), Qt::ISODate);
    ui->lblLatestVersion->setText("Latest snapshot: " + datetime.toLocalTime().toString());

    json_t *url;
    json_t *assets;
    assets = json_object_get(json, "assets");
    if (!json_is_array(assets)) {
        json_decref(json);
        return;
    }
    assets = json_array_get(assets, 0);
    if (!json_is_object(assets)) {
        json_decref(json);
        return;
    }

    url = json_object_get(assets, "browser_download_url");
    if (!json_is_string(url)) {
        json_decref(json);
        return;
    }
    updateUrl = json_string_value(url);

    json_t *size = json_object_get(assets, "size");
    if (!json_is_integer(size)) {
        json_decref(json);
        return;
    }
    dlSize = json_integer_value(size);
    ui->progressBar->setMaximum(dlSize);
    ui->lblProgress->setText("Status: "+QString().setNum(dlSize / 1024.0 / 1024.0, 'f' ,2)+" MiB to download");

    json_decref(json);

    ui->progressBar->setEnabled(true);
    ui->applyButton->setEnabled(true);

    if (autoUpdateFlag) {
        QDateTime testTime = QDateTime(currentDate).addSecs(5*60); // +5 Minutes for compare (guessed build / publish time delay)
        if (datetime > testTime) {
            _plugin_logputs("x64dbg Updater: autoCheck found new snapshot.");
            show();
        } else {
            _plugin_logputs("x64dbg Updater: autoCheck found no new snapshot.");
        }
    }
    autoUpdateFlag = false;
}

void UpdateForm::showEvent(QShowEvent *event) {
    ui->lblCurrentDate->setText("Current file date: " + currentDate.toLocalTime().toString());

    if (autoUpdateFlag == false) {
        checkUpdate(false);
    }
}

UpdateForm::~UpdateForm()
{
    delete ui;
    delete manager;
}

void UpdateForm::on_applyButton_clicked()
{
    TCHAR path[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, path)) { return; }

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

    dlFile.setFileName(QString::fromWCharArray(path).append("x64dbg_snapshot.zip"));
    if (!dlFile.open(QIODevice::ReadWrite)) { return; }

    dlReply = manager->get(QNetworkRequest(QUrl(updateUrl)));
    connect(dlReply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
}
