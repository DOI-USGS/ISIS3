/********************************************************************************
** Form generated from reading UI file 'jigsawdialog.ui'
**
** Created: Sat May 31 08:55:27 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JIGSAWDIALOG_H
#define UI_JIGSAWDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_JigsawDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QPushButton *JigsawSetupButton;
    QPushButton *JigsawRunButton;

    void setupUi(QDialog *JigsawDialog)
    {
        if (JigsawDialog->objectName().isEmpty())
            JigsawDialog->setObjectName(QString::fromUtf8("JigsawDialog"));
        JigsawDialog->resize(400, 300);
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/jigsaw.png"), QSize(), QIcon::Normal, QIcon::Off);
        JigsawDialog->setWindowIcon(icon);
        JigsawDialog->setModal(false);
        buttonBox = new QDialogButtonBox(JigsawDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(50, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        layoutWidget = new QWidget(JigsawDialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(330, 10, 57, 46));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        JigsawSetupButton = new QPushButton(layoutWidget);
        JigsawSetupButton->setObjectName(QString::fromUtf8("JigsawSetupButton"));

        gridLayout->addWidget(JigsawSetupButton, 0, 0, 1, 1);

        JigsawRunButton = new QPushButton(layoutWidget);
        JigsawRunButton->setObjectName(QString::fromUtf8("JigsawRunButton"));

        gridLayout->addWidget(JigsawRunButton, 1, 0, 1, 1);


        retranslateUi(JigsawDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), JigsawDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), JigsawDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(JigsawDialog);
    } // setupUi

    void retranslateUi(QDialog *JigsawDialog)
    {
        JigsawDialog->setWindowTitle(QApplication::translate("JigsawDialog", "Jigsaw", 0, QApplication::UnicodeUTF8));
        JigsawSetupButton->setText(QApplication::translate("JigsawDialog", "&Setup", 0, QApplication::UnicodeUTF8));
        JigsawRunButton->setText(QApplication::translate("JigsawDialog", "&Run", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class JigsawDialog: public Ui_JigsawDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JIGSAWDIALOG_H
