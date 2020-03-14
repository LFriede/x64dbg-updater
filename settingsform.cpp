#include "settingsform.h"
#include "ui_settingsform.h"
#include "x64dbg_updater_global.h"
#include <QFileDialog>

SettingsForm::SettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/images/settings.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowMaximized);
}

void SettingsForm::showEvent(QShowEvent *event) {
    ui->cbAutoCheck->setChecked(globalSettings.autoCheck);
    ui->leManagerPath->setText(globalSettings.managerPath);
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_pbBrowse_clicked()
{
    ui->leManagerPath->setText(QFileDialog::getExistingDirectory(this));
}

void SettingsForm::on_pbSave_clicked()
{
    globalSettings.autoCheck = ui->cbAutoCheck->isChecked();
    strcpy_s(globalSettings.managerPath, MAX_SETTING_SIZE, ui->leManagerPath->text().toStdString().c_str());

    BridgeSettingSetUint("UpdaterPlugin", "AutocheckOnStartup", globalSettings.autoCheck);
    BridgeSettingSet("UpdaterPlugin", "PluginManagerPath", globalSettings.managerPath);

    this->close();
}
