#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();

private slots:
    void on_pbBrowse_clicked();

    void on_pbSave_clicked();

private:
    Ui::SettingsForm *ui;
    void showEvent(QShowEvent* event);
};

#endif // SETTINGSFORM_H
