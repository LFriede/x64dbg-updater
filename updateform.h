#ifndef UPDATEFORM_H
#define UPDATEFORM_H

#include <QWidget>
#include <QtNetwork>
#include <QFile>

namespace Ui {
class UpdateForm;
}

class UpdateForm : public QWidget
{
    Q_OBJECT

public:
    QDateTime currentDate;
    QString updaterPath;
    explicit UpdateForm(QWidget *parent = 0);
    void checkUpdate(bool autoUpdate);
    ~UpdateForm();

public slots:
    void downloadFinished(QNetworkReply *reply);
    void downloadReadyRead();
    void replyFinished(QNetworkReply *reply);

private slots:
    void on_applyButton_clicked();

private:
    Ui::UpdateForm *ui;
    bool autoUpdateFlag;
    QFile dlFile;
    int dlSize;
    QNetworkAccessManager *manager;
    QNetworkReply *dlReply;
    QString updateUrl;
    void showEvent(QShowEvent* event);
};

#endif // UPDATEFORM_H
