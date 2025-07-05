#include "settingsform.h"
#include "ui_settingsform.h"
#include "x64dbg_updater_global.h"
#include <QFileDialog>

SettingsForm::SettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/Default/icons/settings.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowMaximized);
}

void SettingsForm::showEvent(QShowEvent *event) {
    ui->cbAutoCheck->setChecked(globalSettings.autoCheck);
    ui->leManagerPath->setText(globalSettings.managerPath);
    ui->spinDelay->setValue(globalSettings.updateDelayValue);

    switch (globalSettings.updateDelayFactor) {
        case 0: {
            ui->rbHours->setChecked(true);
            break;
        }
        case 1: {
            ui->rbDays->setChecked(true);
            break;
        }
        case 2: {
            ui->rbWeeks->setChecked(true);
            break;
        }
    }
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
    globalSettings.updateDelayValue = ui->spinDelay->value();
    if (ui->rbHours->isChecked()) { globalSettings.updateDelayFactor = 0; }
    if (ui->rbDays->isChecked()) { globalSettings.updateDelayFactor = 1; }
    if (ui->rbWeeks->isChecked()) { globalSettings.updateDelayFactor = 2; }

    BridgeSettingSetUint("UpdaterPlugin", "AutocheckOnStartup", globalSettings.autoCheck);
    BridgeSettingSet("UpdaterPlugin", "PluginManagerPath", globalSettings.managerPath);
    BridgeSettingSetUint("UpdaterPlugin", "DelayValue", globalSettings.updateDelayValue);
    BridgeSettingSetUint("UpdaterPlugin", "DelayFactor", globalSettings.updateDelayFactor);

    this->close();
}
