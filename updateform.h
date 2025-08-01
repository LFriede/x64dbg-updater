#ifndef UPDATEFORM_H
#define UPDATEFORM_H

#include <QWidget>
#include <QFile>
#include <QProcess>

#include "Windows.h"

namespace Ui {
class UpdateForm;
}

class UpdateForm : public QWidget
{
    Q_OBJECT

public:
    QString currentCommitHash;
    explicit UpdateForm(QWidget *parent = 0);
    void checkUpdate();
    ~UpdateForm();

public slots:
    void replyFinished_commits(const unsigned long status, const QString content);
    void plgmgrUpdateServerListFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void plgmgrShowUpdatesFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void updaterProcessError(QProcess::ProcessError error);

private slots:
    void on_pbShowPluginManager_clicked();
    void on_pbUpdateAll_clicked();
    void on_pbUpdateOnExit_clicked();
    void on_pbForceCoreUpdate_clicked();

private:
    Ui::UpdateForm *ui;
    bool autoUpdateFlag;
    bool foundCommitDate;
    QProcess *updaterProcess;
    void showEvent(QShowEvent* event);
    void updateOnExit(bool enabled);
};

#endif // UPDATEFORM_H
