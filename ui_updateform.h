/********************************************************************************
** Form generated from reading UI file 'updateform.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPDATEFORM_H
#define UI_UPDATEFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UpdateForm
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *lblCurrentVersion;
    QLabel *lblLatestVersion;
    QSpacerItem *verticalSpacer_2;
    QProgressBar *progressBar;
    QLabel *lblProgress;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QPushButton *applyButton;

    void setupUi(QWidget *UpdateForm)
    {
        if (UpdateForm->objectName().isEmpty())
            UpdateForm->setObjectName(QStringLiteral("UpdateForm"));
        UpdateForm->resize(400, 175);
        verticalLayout = new QVBoxLayout(UpdateForm);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        lblCurrentVersion = new QLabel(UpdateForm);
        lblCurrentVersion->setObjectName(QStringLiteral("lblCurrentVersion"));

        verticalLayout->addWidget(lblCurrentVersion);

        lblLatestVersion = new QLabel(UpdateForm);
        lblLatestVersion->setObjectName(QStringLiteral("lblLatestVersion"));

        verticalLayout->addWidget(lblLatestVersion);

        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        progressBar = new QProgressBar(UpdateForm);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setEnabled(false);
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        lblProgress = new QLabel(UpdateForm);
        lblProgress->setObjectName(QStringLiteral("lblProgress"));

        verticalLayout->addWidget(lblProgress);

        label = new QLabel(UpdateForm);
        label->setObjectName(QStringLiteral("label"));
        label->setOpenExternalLinks(true);

        verticalLayout->addWidget(label);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        applyButton = new QPushButton(UpdateForm);
        applyButton->setObjectName(QStringLiteral("applyButton"));
        applyButton->setEnabled(true);

        verticalLayout->addWidget(applyButton);


        retranslateUi(UpdateForm);

        QMetaObject::connectSlotsByName(UpdateForm);
    } // setupUi

    void retranslateUi(QWidget *UpdateForm)
    {
        UpdateForm->setWindowTitle(QApplication::translate("UpdateForm", "x64dbg Updater", 0));
        lblCurrentVersion->setText(QApplication::translate("UpdateForm", "Current commit:", 0));
        lblLatestVersion->setText(QApplication::translate("UpdateForm", "Latest commit:", 0));
        lblProgress->setText(QApplication::translate("UpdateForm", "Status:", 0));
        label->setText(QApplication::translate("UpdateForm", "Show latest <a href=\"https://github.com/x64dbg/x64dbg/commits\">commits</a> on GitHub.", 0));
        applyButton->setText(QApplication::translate("UpdateForm", "Apply update", 0));
    } // retranslateUi

};

namespace Ui {
    class UpdateForm: public Ui_UpdateForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATEFORM_H
