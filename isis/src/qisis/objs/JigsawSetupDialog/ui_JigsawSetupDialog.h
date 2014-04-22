/********************************************************************************
** Form generated from reading UI file 'JigsawSetupDialog.ui'
**
** Created: Mon Apr 21 17:15:24 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JIGSAWSETUPDIALOG_H
#define UI_JIGSAWSETUPDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_JigsawSetupDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *JigsawSetup;
    QWidget *tab;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_17;
    QComboBox *PointingCombobox;
    QComboBox *SpacecraftCombobox;
    QLabel *label_3;
    QSpinBox *CKSolveDegree;
    QLabel *label_4;
    QSpacerItem *verticalSpacer;
    QCheckBox *Twist;
    QLabel *label;
    QSpinBox *CKDegree;
    QLabel *label_16;
    QSpinBox *SPKDegree;
    QComboBox *SolveMethod;
    QSpinBox *SPKSolveDegree;
    QLabel *label_18;
    QLabel *label_15;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_2;
    QCheckBox *ObservationMode;
    QCheckBox *Radius;
    QCheckBox *UpdateCubeLabel;
    QCheckBox *ErrorPropagation;
    QCheckBox *OutlierRejection;
    QLineEdit *OutlierRejectionMultiplier;
    QLabel *label_21;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout_6;
    QLabel *label_22;
    QComboBox *ControlNetCombo;
    QWidget *layoutWidget3;
    QGridLayout *gridLayout_4;
    QLabel *label_2;
    QLineEdit *Sigma0Edit;
    QLabel *label_19;
    QLineEdit *MaxIterationsEdit;
    QLabel *label_20;
    QDialogButtonBox *buttonBox_2;
    QWidget *tab_5;
    QWidget *layoutWidget_2;
    QGridLayout *gridLayout_3;
    QLineEdit *SpacecraftPositionSigma;
    QLabel *label_9;
    QLineEdit *SensorAccelerationSigma;
    QLabel *label_11;
    QLabel *label_14;
    QLabel *label_5;
    QLineEdit *SensorAngleSigma;
    QLineEdit *PointLongitudeSigma;
    QLabel *label_6;
    QLineEdit *PointLatitudeSigma;
    QLineEdit *SpacecraftVelocitySigma;
    QLabel *label_10;
    QLabel *label_7;
    QLineEdit *PointRadiusSigma;
    QLabel *label_12;
    QLabel *label_8;
    QLineEdit *SensorVelocitySigma;
    QLabel *label_13;
    QLineEdit *SpacecraftAccelerationSigma;
    QWidget *tab_4;
    QWidget *tab_2;
    QWidget *tab_3;

    void setupUi(QDialog *JigsawSetupDialog)
    {
        if (JigsawSetupDialog->objectName().isEmpty())
            JigsawSetupDialog->setObjectName(QString::fromUtf8("JigsawSetupDialog"));
        JigsawSetupDialog->resize(668, 595);
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/jigsaw.png"), QSize(), QIcon::Normal, QIcon::Off);
        JigsawSetupDialog->setWindowIcon(icon);
        JigsawSetupDialog->setInputMethodHints(Qt::ImhNone);
        buttonBox = new QDialogButtonBox(JigsawSetupDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(320, 560, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        JigsawSetup = new QTabWidget(JigsawSetupDialog);
        JigsawSetup->setObjectName(QString::fromUtf8("JigsawSetup"));
        JigsawSetup->setGeometry(QRect(0, 0, 671, 561));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        layoutWidget = new QWidget(tab);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 190, 310, 338));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 5, 0, 1, 2);

        label_17 = new QLabel(layoutWidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout->addWidget(label_17, 14, 0, 1, 1);

        PointingCombobox = new QComboBox(layoutWidget);
        PointingCombobox->setObjectName(QString::fromUtf8("PointingCombobox"));
        PointingCombobox->setMaximumSize(QSize(1000, 16777215));
        PointingCombobox->setFrame(true);

        gridLayout->addWidget(PointingCombobox, 12, 0, 1, 2);

        SpacecraftCombobox = new QComboBox(layoutWidget);
        SpacecraftCombobox->setObjectName(QString::fromUtf8("SpacecraftCombobox"));
        SpacecraftCombobox->setMaximumSize(QSize(201, 16777215));

        gridLayout->addWidget(SpacecraftCombobox, 7, 0, 1, 2);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 9, 0, 1, 1);

        CKSolveDegree = new QSpinBox(layoutWidget);
        CKSolveDegree->setObjectName(QString::fromUtf8("CKSolveDegree"));
        CKSolveDegree->setMinimum(3);
        CKSolveDegree->setValue(3);

        gridLayout->addWidget(CKSolveDegree, 15, 1, 1, 1);

        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 8, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 10, 0, 1, 2);

        Twist = new QCheckBox(layoutWidget);
        Twist->setObjectName(QString::fromUtf8("Twist"));

        gridLayout->addWidget(Twist, 13, 0, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        CKDegree = new QSpinBox(layoutWidget);
        CKDegree->setObjectName(QString::fromUtf8("CKDegree"));
        CKDegree->setMinimum(3);
        CKDegree->setValue(3);

        gridLayout->addWidget(CKDegree, 14, 1, 1, 1);

        label_16 = new QLabel(layoutWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setScaledContents(false);
        label_16->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_16, 11, 0, 1, 2);

        SPKDegree = new QSpinBox(layoutWidget);
        SPKDegree->setObjectName(QString::fromUtf8("SPKDegree"));
        SPKDegree->setMinimum(3);
        SPKDegree->setValue(3);

        gridLayout->addWidget(SPKDegree, 8, 1, 1, 1);

        SolveMethod = new QComboBox(layoutWidget);
        SolveMethod->setObjectName(QString::fromUtf8("SolveMethod"));

        gridLayout->addWidget(SolveMethod, 1, 0, 1, 1);

        SPKSolveDegree = new QSpinBox(layoutWidget);
        SPKSolveDegree->setObjectName(QString::fromUtf8("SPKSolveDegree"));
        SPKSolveDegree->setMinimum(3);
        SPKSolveDegree->setValue(3);

        gridLayout->addWidget(SPKSolveDegree, 9, 1, 1, 1);

        label_18 = new QLabel(layoutWidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout->addWidget(label_18, 15, 0, 1, 1);

        label_15 = new QLabel(layoutWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setScaledContents(false);
        label_15->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_15, 6, 0, 1, 2);

        layoutWidget1 = new QWidget(tab);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(11, 11, 222, 139));
        gridLayout_2 = new QGridLayout(layoutWidget1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        ObservationMode = new QCheckBox(layoutWidget1);
        ObservationMode->setObjectName(QString::fromUtf8("ObservationMode"));

        gridLayout_2->addWidget(ObservationMode, 0, 0, 1, 2);

        Radius = new QCheckBox(layoutWidget1);
        Radius->setObjectName(QString::fromUtf8("Radius"));

        gridLayout_2->addWidget(Radius, 1, 0, 1, 1);

        UpdateCubeLabel = new QCheckBox(layoutWidget1);
        UpdateCubeLabel->setObjectName(QString::fromUtf8("UpdateCubeLabel"));

        gridLayout_2->addWidget(UpdateCubeLabel, 2, 0, 1, 2);

        ErrorPropagation = new QCheckBox(layoutWidget1);
        ErrorPropagation->setObjectName(QString::fromUtf8("ErrorPropagation"));
        ErrorPropagation->setMinimumSize(QSize(74, 0));

        gridLayout_2->addWidget(ErrorPropagation, 3, 0, 1, 2);

        OutlierRejection = new QCheckBox(layoutWidget1);
        OutlierRejection->setObjectName(QString::fromUtf8("OutlierRejection"));
        OutlierRejection->setMinimumSize(QSize(74, 0));

        gridLayout_2->addWidget(OutlierRejection, 5, 0, 1, 1);

        OutlierRejectionMultiplier = new QLineEdit(layoutWidget1);
        OutlierRejectionMultiplier->setObjectName(QString::fromUtf8("OutlierRejectionMultiplier"));

        gridLayout_2->addWidget(OutlierRejectionMultiplier, 5, 1, 1, 2);

        label_21 = new QLabel(layoutWidget1);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_21, 4, 1, 1, 2);

        layoutWidget2 = new QWidget(tab);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(250, 10, 401, 39));
        gridLayout_6 = new QGridLayout(layoutWidget2);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setContentsMargins(0, 0, 0, 0);
        label_22 = new QLabel(layoutWidget2);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(label_22, 0, 0, 1, 1);

        ControlNetCombo = new QComboBox(layoutWidget2);
        ControlNetCombo->setObjectName(QString::fromUtf8("ControlNetCombo"));

        gridLayout_6->addWidget(ControlNetCombo, 1, 0, 1, 1);

        layoutWidget3 = new QWidget(tab);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(390, 190, 222, 65));
        gridLayout_4 = new QGridLayout(layoutWidget3);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(layoutWidget3);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_4->addWidget(label_2, 1, 0, 1, 1);

        Sigma0Edit = new QLineEdit(layoutWidget3);
        Sigma0Edit->setObjectName(QString::fromUtf8("Sigma0Edit"));

        gridLayout_4->addWidget(Sigma0Edit, 1, 1, 1, 1);

        label_19 = new QLabel(layoutWidget3);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout_4->addWidget(label_19, 2, 0, 1, 1);

        MaxIterationsEdit = new QLineEdit(layoutWidget3);
        MaxIterationsEdit->setObjectName(QString::fromUtf8("MaxIterationsEdit"));

        gridLayout_4->addWidget(MaxIterationsEdit, 2, 1, 1, 1);

        label_20 = new QLabel(layoutWidget3);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(label_20, 0, 0, 1, 2);

        buttonBox_2 = new QDialogButtonBox(tab);
        buttonBox_2->setObjectName(QString::fromUtf8("buttonBox_2"));
        buttonBox_2->setGeometry(QRect(320, 560, 341, 32));
        buttonBox_2->setOrientation(Qt::Horizontal);
        buttonBox_2->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        JigsawSetup->addTab(tab, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        layoutWidget_2 = new QWidget(tab_5);
        layoutWidget_2->setObjectName(QString::fromUtf8("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(10, 10, 267, 247));
        gridLayout_3 = new QGridLayout(layoutWidget_2);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        SpacecraftPositionSigma = new QLineEdit(layoutWidget_2);
        SpacecraftPositionSigma->setObjectName(QString::fromUtf8("SpacecraftPositionSigma"));

        gridLayout_3->addWidget(SpacecraftPositionSigma, 4, 2, 1, 1);

        label_9 = new QLabel(layoutWidget_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_3->addWidget(label_9, 5, 0, 1, 1);

        SensorAccelerationSigma = new QLineEdit(layoutWidget_2);
        SensorAccelerationSigma->setObjectName(QString::fromUtf8("SensorAccelerationSigma"));

        gridLayout_3->addWidget(SensorAccelerationSigma, 9, 2, 1, 1);

        label_11 = new QLabel(layoutWidget_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_3->addWidget(label_11, 7, 0, 1, 1);

        label_14 = new QLabel(layoutWidget_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(label_14, 0, 0, 1, 3);

        label_5 = new QLabel(layoutWidget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_3->addWidget(label_5, 1, 0, 1, 1);

        SensorAngleSigma = new QLineEdit(layoutWidget_2);
        SensorAngleSigma->setObjectName(QString::fromUtf8("SensorAngleSigma"));

        gridLayout_3->addWidget(SensorAngleSigma, 7, 2, 1, 1);

        PointLongitudeSigma = new QLineEdit(layoutWidget_2);
        PointLongitudeSigma->setObjectName(QString::fromUtf8("PointLongitudeSigma"));

        gridLayout_3->addWidget(PointLongitudeSigma, 2, 2, 1, 1);

        label_6 = new QLabel(layoutWidget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_3->addWidget(label_6, 2, 0, 1, 1);

        PointLatitudeSigma = new QLineEdit(layoutWidget_2);
        PointLatitudeSigma->setObjectName(QString::fromUtf8("PointLatitudeSigma"));

        gridLayout_3->addWidget(PointLatitudeSigma, 1, 2, 1, 1);

        SpacecraftVelocitySigma = new QLineEdit(layoutWidget_2);
        SpacecraftVelocitySigma->setObjectName(QString::fromUtf8("SpacecraftVelocitySigma"));

        gridLayout_3->addWidget(SpacecraftVelocitySigma, 5, 2, 1, 1);

        label_10 = new QLabel(layoutWidget_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_3->addWidget(label_10, 6, 0, 1, 2);

        label_7 = new QLabel(layoutWidget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_3->addWidget(label_7, 3, 0, 1, 1);

        PointRadiusSigma = new QLineEdit(layoutWidget_2);
        PointRadiusSigma->setObjectName(QString::fromUtf8("PointRadiusSigma"));

        gridLayout_3->addWidget(PointRadiusSigma, 3, 2, 1, 1);

        label_12 = new QLabel(layoutWidget_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_3->addWidget(label_12, 8, 0, 1, 1);

        label_8 = new QLabel(layoutWidget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_3->addWidget(label_8, 4, 0, 1, 1);

        SensorVelocitySigma = new QLineEdit(layoutWidget_2);
        SensorVelocitySigma->setObjectName(QString::fromUtf8("SensorVelocitySigma"));

        gridLayout_3->addWidget(SensorVelocitySigma, 8, 2, 1, 1);

        label_13 = new QLabel(layoutWidget_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_3->addWidget(label_13, 9, 0, 1, 1);

        SpacecraftAccelerationSigma = new QLineEdit(layoutWidget_2);
        SpacecraftAccelerationSigma->setObjectName(QString::fromUtf8("SpacecraftAccelerationSigma"));

        gridLayout_3->addWidget(SpacecraftAccelerationSigma, 6, 2, 1, 1);

        JigsawSetup->addTab(tab_5, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        JigsawSetup->addTab(tab_4, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        JigsawSetup->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        JigsawSetup->addTab(tab_3, QString());
        QWidget::setTabOrder(ObservationMode, Radius);
        QWidget::setTabOrder(Radius, UpdateCubeLabel);
        QWidget::setTabOrder(UpdateCubeLabel, ErrorPropagation);
        QWidget::setTabOrder(ErrorPropagation, OutlierRejection);
        QWidget::setTabOrder(OutlierRejection, OutlierRejectionMultiplier);
        QWidget::setTabOrder(OutlierRejectionMultiplier, SolveMethod);
        QWidget::setTabOrder(SolveMethod, SpacecraftCombobox);
        QWidget::setTabOrder(SpacecraftCombobox, SPKDegree);
        QWidget::setTabOrder(SPKDegree, SPKSolveDegree);
        QWidget::setTabOrder(SPKSolveDegree, PointingCombobox);
        QWidget::setTabOrder(PointingCombobox, Twist);
        QWidget::setTabOrder(Twist, CKDegree);
        QWidget::setTabOrder(CKDegree, CKSolveDegree);
        QWidget::setTabOrder(CKSolveDegree, ControlNetCombo);
        QWidget::setTabOrder(ControlNetCombo, Sigma0Edit);
        QWidget::setTabOrder(Sigma0Edit, MaxIterationsEdit);
        QWidget::setTabOrder(MaxIterationsEdit, PointLatitudeSigma);
        QWidget::setTabOrder(PointLatitudeSigma, PointLongitudeSigma);
        QWidget::setTabOrder(PointLongitudeSigma, PointRadiusSigma);
        QWidget::setTabOrder(PointRadiusSigma, SpacecraftPositionSigma);
        QWidget::setTabOrder(SpacecraftPositionSigma, SpacecraftVelocitySigma);
        QWidget::setTabOrder(SpacecraftVelocitySigma, SpacecraftAccelerationSigma);
        QWidget::setTabOrder(SpacecraftAccelerationSigma, SensorAngleSigma);
        QWidget::setTabOrder(SensorAngleSigma, SensorVelocitySigma);
        QWidget::setTabOrder(SensorVelocitySigma, SensorAccelerationSigma);
        QWidget::setTabOrder(SensorAccelerationSigma, buttonBox);
        QWidget::setTabOrder(buttonBox, JigsawSetup);

        retranslateUi(JigsawSetupDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), JigsawSetupDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), JigsawSetupDialog, SLOT(reject()));

        JigsawSetup->setCurrentIndex(0);
        PointingCombobox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(JigsawSetupDialog);
    } // setupUi

    void retranslateUi(QDialog *JigsawSetupDialog)
    {
        JigsawSetupDialog->setWindowTitle(QApplication::translate("JigsawSetupDialog", "Jigsaw Setup", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("JigsawSetupDialog", "CKDegree", 0, QApplication::UnicodeUTF8));
        PointingCombobox->clear();
        PointingCombobox->insertItems(0, QStringList()
         << QApplication::translate("JigsawSetupDialog", "ANGLES", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "NONE", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "VELOCITIES", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "ACCELERATION", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "ALL", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        PointingCombobox->setToolTip(QApplication::translate("JigsawSetupDialog", "<html><head/><body><p>Camera Pointing Options</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        SpacecraftCombobox->clear();
        SpacecraftCombobox->insertItems(0, QStringList()
         << QApplication::translate("JigsawSetupDialog", "NONE", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "POSITION", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "VELOCITY", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "ACCELERATION", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "ALL", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        SpacecraftCombobox->setToolTip(QApplication::translate("JigsawSetupDialog", "<html><head/><body><p>Spacecraft Solve Options</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("JigsawSetupDialog", "SPKSolveDegree", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("JigsawSetupDialog", "SPKDegree", 0, QApplication::UnicodeUTF8));
        Twist->setText(QApplication::translate("JigsawSetupDialog", "Twist", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("JigsawSetupDialog", "Solve Method", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("JigsawSetupDialog", "Camera Pointing Solve Options", 0, QApplication::UnicodeUTF8));
        SolveMethod->clear();
        SolveMethod->insertItems(0, QStringList()
         << QApplication::translate("JigsawSetupDialog", "SPARSE", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("JigsawSetupDialog", "SPECIALK", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        SolveMethod->setToolTip(QApplication::translate("JigsawSetupDialog", "<html><head/><body><p>Solve method</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_18->setText(QApplication::translate("JigsawSetupDialog", "CKSolveDegree", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("JigsawSetupDialog", "Spacecraft Solve Options", 0, QApplication::UnicodeUTF8));
        ObservationMode->setText(QApplication::translate("JigsawSetupDialog", "Observation Mode", 0, QApplication::UnicodeUTF8));
        Radius->setText(QApplication::translate("JigsawSetupDialog", "Radius", 0, QApplication::UnicodeUTF8));
        UpdateCubeLabel->setText(QApplication::translate("JigsawSetupDialog", "Update Cube Label", 0, QApplication::UnicodeUTF8));
        ErrorPropagation->setText(QApplication::translate("JigsawSetupDialog", "Error Propagation", 0, QApplication::UnicodeUTF8));
        OutlierRejection->setText(QApplication::translate("JigsawSetupDialog", "Outlier Rejection", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        OutlierRejectionMultiplier->setToolTip(QApplication::translate("JigsawSetupDialog", "<html><head/><body><p>Outlier Rejection Multiplier</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        OutlierRejectionMultiplier->setText(QApplication::translate("JigsawSetupDialog", "3", 0, QApplication::UnicodeUTF8));
        OutlierRejectionMultiplier->setPlaceholderText(QString());
        label_21->setText(QApplication::translate("JigsawSetupDialog", "Multiplier", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("JigsawSetupDialog", "Control Network", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("JigsawSetupDialog", "Sigma0", 0, QApplication::UnicodeUTF8));
        Sigma0Edit->setText(QApplication::translate("JigsawSetupDialog", "1.0e-10", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("JigsawSetupDialog", "Maximum Iterations", 0, QApplication::UnicodeUTF8));
        MaxIterationsEdit->setText(QApplication::translate("JigsawSetupDialog", "50", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("JigsawSetupDialog", "Convergence Criteria", 0, QApplication::UnicodeUTF8));
        JigsawSetup->setTabText(JigsawSetup->indexOf(tab), QApplication::translate("JigsawSetupDialog", "General", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("JigsawSetupDialog", "Spacecraft Velocity Sigma", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("JigsawSetupDialog", "Camera Angles Sigma", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("JigsawSetupDialog", "Global Parameter Uncertainties", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("JigsawSetupDialog", "Point Latitude Sigma", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("JigsawSetupDialog", "Point Longitude Sigma", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("JigsawSetupDialog", "Spacecraft Acceleration Sigma", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("JigsawSetupDialog", "Point Radius Sigma", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("JigsawSetupDialog", "Camera Velocity Sigma", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("JigsawSetupDialog", "Spacecraft Position Sigma", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("JigsawSetupDialog", "Camera Acceleration Sigma", 0, QApplication::UnicodeUTF8));
        JigsawSetup->setTabText(JigsawSetup->indexOf(tab_5), QApplication::translate("JigsawSetupDialog", "Weighting", 0, QApplication::UnicodeUTF8));
        JigsawSetup->setTabText(JigsawSetup->indexOf(tab_4), QApplication::translate("JigsawSetupDialog", "Maximum Liklihood", 0, QApplication::UnicodeUTF8));
        JigsawSetup->setTabText(JigsawSetup->indexOf(tab_2), QApplication::translate("JigsawSetupDialog", "Self-Calibration", 0, QApplication::UnicodeUTF8));
        JigsawSetup->setTabText(JigsawSetup->indexOf(tab_3), QApplication::translate("JigsawSetupDialog", "Target Body", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class JigsawSetupDialog: public Ui_JigsawSetupDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JIGSAWSETUPDIALOG_H
