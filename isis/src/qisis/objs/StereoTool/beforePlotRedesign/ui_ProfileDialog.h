/********************************************************************************
** Form generated from reading UI file 'ProfileDialog.ui'
**
** Created: Wed Feb 29 11:22:18 2012
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEDIALOG_H
#define UI_PROFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProfileDialog
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QPushButton *createStartButton;
    QPushButton *createEndButton;
    QHBoxLayout *horizontalLayout;
    QPushButton *helpButton;
    QPushButton *profileButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *ProfileDialog)
    {
        if (ProfileDialog->objectName().isEmpty())
            ProfileDialog->setObjectName(QString::fromUtf8("ProfileDialog"));
        ProfileDialog->resize(271, 168);
        widget = new QWidget(ProfileDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 32, 249, 103));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        createStartButton = new QPushButton(widget);
        createStartButton->setObjectName(QString::fromUtf8("createStartButton"));

        verticalLayout->addWidget(createStartButton);

        createEndButton = new QPushButton(widget);
        createEndButton->setObjectName(QString::fromUtf8("createEndButton"));

        verticalLayout->addWidget(createEndButton);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        helpButton = new QPushButton(widget);
        helpButton->setObjectName(QString::fromUtf8("helpButton"));

        horizontalLayout->addWidget(helpButton);

        profileButton = new QPushButton(widget);
        profileButton->setObjectName(QString::fromUtf8("profileButton"));
        profileButton->setEnabled(false);

        horizontalLayout->addWidget(profileButton);

        cancelButton = new QPushButton(widget);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ProfileDialog);
        QObject::connect(profileButton, SIGNAL(clicked()), ProfileDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), ProfileDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ProfileDialog);
    } // setupUi

    void retranslateUi(QDialog *ProfileDialog)
    {
        ProfileDialog->setWindowTitle(QApplication::translate("ProfileDialog", "Select Profile Endpoints", 0, QApplication::UnicodeUTF8));
        createStartButton->setText(QApplication::translate("ProfileDialog", "Create Start Point", 0, QApplication::UnicodeUTF8));
        createEndButton->setText(QApplication::translate("ProfileDialog", "Create End Point", 0, QApplication::UnicodeUTF8));
        helpButton->setText(QApplication::translate("ProfileDialog", "Help", 0, QApplication::UnicodeUTF8));
        profileButton->setText(QApplication::translate("ProfileDialog", "Profile", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("ProfileDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ProfileDialog: public Ui_ProfileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEDIALOG_H
