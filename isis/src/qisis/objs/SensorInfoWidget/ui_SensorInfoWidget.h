/********************************************************************************
** Form generated from reading UI file 'SensorInfoWidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SENSORINFOWIDGET_H
#define UI_SENSORINFOWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SensorInfoWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *targetImage;
    QLabel *spacecraftlabel;
    QSpacerItem *verticalSpacer;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QLabel *poleDeclinationLabel;
    QLabel *label_2;
    QLabel *poleRightAscensionLabel;
    QSpacerItem *horizontalSpacer_4;
    QWidget *tab_2;
    QWidget *layoutWidget_2;
    QGridLayout *gridLayout_2;
    QLabel *polePMOffsetLabel;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_6;
    QWidget *tab_3;
    QWidget *layoutWidget_3;
    QGridLayout *gridLayout_3;
    QSpacerItem *horizontalSpacer_11;
    QLabel *bRadiiLabel;
    QLabel *meanRadiiLabel;
    QLabel *cRadiiLabel;
    QLabel *meanLabel;
    QLabel *aRadiiLabel;
    QLabel *bRadiiUnitsLabel;
    QLabel *meanRadiiUnitsLabel;
    QLabel *cRadiiUnitsLabel;
    QLabel *aLabel;
    QLabel *cLabel;
    QLabel *aRadiiUnitsLabel;
    QLabel *bLabel;
    QLabel *label_9;
    QLabel *label_10;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_7;
    QLabel *label_11;
    QLabel *label_12;

    void setupUi(QFrame *SensorInfoWidget)
    {
        if (SensorInfoWidget->objectName().isEmpty())
            SensorInfoWidget->setObjectName(QString::fromUtf8("SensorInfoWidget"));
        SensorInfoWidget->resize(420, 581);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SensorInfoWidget->sizePolicy().hasHeightForWidth());
        SensorInfoWidget->setSizePolicy(sizePolicy);
        SensorInfoWidget->setMinimumSize(QSize(420, 0));
        SensorInfoWidget->setMaximumSize(QSize(420, 16777215));
        verticalLayout = new QVBoxLayout(SensorInfoWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        targetImage = new QLabel(SensorInfoWidget);
        targetImage->setObjectName(QString::fromUtf8("targetImage"));
        sizePolicy.setHeightForWidth(targetImage->sizePolicy().hasHeightForWidth());
        targetImage->setSizePolicy(sizePolicy);
        targetImage->setMinimumSize(QSize(391, 181));
        targetImage->setMaximumSize(QSize(391, 181));
        targetImage->setFrameShape(QFrame::Panel);
        targetImage->setFrameShadow(QFrame::Sunken);
        targetImage->setLineWidth(2);
        targetImage->setScaledContents(true);

        verticalLayout->addWidget(targetImage);

        spacecraftlabel = new QLabel(SensorInfoWidget);
        spacecraftlabel->setObjectName(QString::fromUtf8("spacecraftlabel"));

        verticalLayout->addWidget(spacecraftlabel);

        verticalSpacer = new QSpacerItem(20, 37, QSizePolicy::Minimum, QSizePolicy::Preferred);

        verticalLayout->addItem(verticalSpacer);

        tabWidget = new QTabWidget(SensorInfoWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMinimumSize(QSize(391, 231));
        tabWidget->setMaximumSize(QSize(391, 231));
        tabWidget->setTabPosition(QTabWidget::South);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        layoutWidget = new QWidget(tab);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 524, 261));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        poleDeclinationLabel = new QLabel(layoutWidget);
        poleDeclinationLabel->setObjectName(QString::fromUtf8("poleDeclinationLabel"));
        QFont font1;
        font1.setPointSize(10);
        poleDeclinationLabel->setFont(font1);
        poleDeclinationLabel->setWordWrap(true);

        gridLayout->addWidget(poleDeclinationLabel, 3, 0, 1, 3);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        gridLayout->addWidget(label_2, 2, 0, 1, 2);

        poleRightAscensionLabel = new QLabel(layoutWidget);
        poleRightAscensionLabel->setObjectName(QString::fromUtf8("poleRightAscensionLabel"));
        poleRightAscensionLabel->setFont(font1);
        poleRightAscensionLabel->setWordWrap(true);

        gridLayout->addWidget(poleRightAscensionLabel, 1, 0, 1, 3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 2, 2, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        layoutWidget_2 = new QWidget(tab_2);
        layoutWidget_2->setObjectName(QString::fromUtf8("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(10, 10, 351, 181));
        gridLayout_2 = new QGridLayout(layoutWidget_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        polePMOffsetLabel = new QLabel(layoutWidget_2);
        polePMOffsetLabel->setObjectName(QString::fromUtf8("polePMOffsetLabel"));
        polePMOffsetLabel->setFont(font1);
        polePMOffsetLabel->setWordWrap(true);

        gridLayout_2->addWidget(polePMOffsetLabel, 1, 0, 1, 3);

        label_6 = new QLabel(layoutWidget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setFont(font);

        gridLayout_2->addWidget(label_6, 0, 0, 1, 2);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 0, 2, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        layoutWidget_3 = new QWidget(tab_3);
        layoutWidget_3->setObjectName(QString::fromUtf8("layoutWidget_3"));
        layoutWidget_3->setGeometry(QRect(20, 20, 351, 161));
        gridLayout_3 = new QGridLayout(layoutWidget_3);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_11, 0, 2, 1, 1);

        bRadiiLabel = new QLabel(layoutWidget_3);
        bRadiiLabel->setObjectName(QString::fromUtf8("bRadiiLabel"));
        QFont font2;
        font2.setPointSize(16);
        bRadiiLabel->setFont(font2);
        bRadiiLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(bRadiiLabel, 2, 2, 1, 1);

        meanRadiiLabel = new QLabel(layoutWidget_3);
        meanRadiiLabel->setObjectName(QString::fromUtf8("meanRadiiLabel"));
        meanRadiiLabel->setFont(font2);
        meanRadiiLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(meanRadiiLabel, 4, 2, 1, 1);

        cRadiiLabel = new QLabel(layoutWidget_3);
        cRadiiLabel->setObjectName(QString::fromUtf8("cRadiiLabel"));
        cRadiiLabel->setFont(font2);
        cRadiiLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(cRadiiLabel, 3, 2, 1, 1);

        meanLabel = new QLabel(layoutWidget_3);
        meanLabel->setObjectName(QString::fromUtf8("meanLabel"));
        QFont font3;
        font3.setPointSize(16);
        font3.setBold(true);
        font3.setWeight(75);
        meanLabel->setFont(font3);
        meanLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(meanLabel, 4, 0, 1, 1);

        aRadiiLabel = new QLabel(layoutWidget_3);
        aRadiiLabel->setObjectName(QString::fromUtf8("aRadiiLabel"));
        aRadiiLabel->setFont(font2);
        aRadiiLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(aRadiiLabel, 1, 2, 1, 1);

        bRadiiUnitsLabel = new QLabel(layoutWidget_3);
        bRadiiUnitsLabel->setObjectName(QString::fromUtf8("bRadiiUnitsLabel"));
        bRadiiUnitsLabel->setFont(font2);
        bRadiiUnitsLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(bRadiiUnitsLabel, 2, 3, 1, 1);

        meanRadiiUnitsLabel = new QLabel(layoutWidget_3);
        meanRadiiUnitsLabel->setObjectName(QString::fromUtf8("meanRadiiUnitsLabel"));
        meanRadiiUnitsLabel->setFont(font2);
        meanRadiiUnitsLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(meanRadiiUnitsLabel, 4, 3, 1, 1);

        cRadiiUnitsLabel = new QLabel(layoutWidget_3);
        cRadiiUnitsLabel->setObjectName(QString::fromUtf8("cRadiiUnitsLabel"));
        cRadiiUnitsLabel->setFont(font2);
        cRadiiUnitsLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(cRadiiUnitsLabel, 3, 3, 1, 1);

        aLabel = new QLabel(layoutWidget_3);
        aLabel->setObjectName(QString::fromUtf8("aLabel"));
        aLabel->setFont(font3);
        aLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(aLabel, 1, 0, 1, 1);

        cLabel = new QLabel(layoutWidget_3);
        cLabel->setObjectName(QString::fromUtf8("cLabel"));
        cLabel->setFont(font3);
        cLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(cLabel, 3, 0, 1, 1);

        aRadiiUnitsLabel = new QLabel(layoutWidget_3);
        aRadiiUnitsLabel->setObjectName(QString::fromUtf8("aRadiiUnitsLabel"));
        aRadiiUnitsLabel->setFont(font2);
        aRadiiUnitsLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(aRadiiUnitsLabel, 1, 3, 1, 1);

        bLabel = new QLabel(layoutWidget_3);
        bLabel->setObjectName(QString::fromUtf8("bLabel"));
        bLabel->setFont(font3);
        bLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(bLabel, 2, 0, 1, 1);

        label_9 = new QLabel(layoutWidget_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setFont(font3);

        gridLayout_3->addWidget(label_9, 0, 0, 1, 2);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        label_10 = new QLabel(SensorInfoWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        verticalLayout->addWidget(label_10);

        verticalSpacer_2 = new QSpacerItem(20, 37, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        label_7 = new QLabel(SensorInfoWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setWordWrap(true);

        verticalLayout->addWidget(label_7);

        label_11 = new QLabel(SensorInfoWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        verticalLayout->addWidget(label_11);

        label_12 = new QLabel(SensorInfoWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout->addWidget(label_12);


        retranslateUi(SensorInfoWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SensorInfoWidget);
    } // setupUi

    void retranslateUi(QFrame *SensorInfoWidget)
    {
        SensorInfoWidget->setWindowTitle(QApplication::translate("SensorInfoWidget", "DockWidget", 0, QApplication::UnicodeUTF8));
        targetImage->setText(QString());
        spacecraftlabel->setText(QApplication::translate("SensorInfoWidget", "Spacecraft", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SensorInfoWidget", "Right Ascension", 0, QApplication::UnicodeUTF8));
        poleDeclinationLabel->setText(QApplication::translate("SensorInfoWidget", "Now is the time for all good men to come to the aid of their countrymen", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SensorInfoWidget", "Declination", 0, QApplication::UnicodeUTF8));
        poleRightAscensionLabel->setText(QApplication::translate("SensorInfoWidget", "Now is the time for all good men to come to the aid of their countrymen", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SensorInfoWidget", "Specifications", 0, QApplication::UnicodeUTF8));
        polePMOffsetLabel->setText(QApplication::translate("SensorInfoWidget", "Now is the time for all good men to come to the aid of their countrymen", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("SensorInfoWidget", "Prime Meridian (W)", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SensorInfoWidget", "Calibration", 0, QApplication::UnicodeUTF8));
        bRadiiLabel->setText(QApplication::translate("SensorInfoWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        meanRadiiLabel->setText(QApplication::translate("SensorInfoWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        cRadiiLabel->setText(QApplication::translate("SensorInfoWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        meanLabel->setText(QApplication::translate("SensorInfoWidget", "mean", 0, QApplication::UnicodeUTF8));
        aRadiiLabel->setText(QApplication::translate("SensorInfoWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        bRadiiUnitsLabel->setText(QApplication::translate("SensorInfoWidget", "km", 0, QApplication::UnicodeUTF8));
        meanRadiiUnitsLabel->setText(QApplication::translate("SensorInfoWidget", "km", 0, QApplication::UnicodeUTF8));
        cRadiiUnitsLabel->setText(QApplication::translate("SensorInfoWidget", "km", 0, QApplication::UnicodeUTF8));
        aLabel->setText(QApplication::translate("SensorInfoWidget", "a", 0, QApplication::UnicodeUTF8));
        cLabel->setText(QApplication::translate("SensorInfoWidget", "c", 0, QApplication::UnicodeUTF8));
        aRadiiUnitsLabel->setText(QApplication::translate("SensorInfoWidget", "km", 0, QApplication::UnicodeUTF8));
        bLabel->setText(QApplication::translate("SensorInfoWidget", "b", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("SensorInfoWidget", "Radii", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("SensorInfoWidget", "What else?", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("SensorInfoWidget", "where", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("SensorInfoWidget", "T = interval in Julien centuries (of 36525 days) from the standard epoch.", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("SensorInfoWidget", "d = interval in days from the standard epoch.", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("SensorInfoWidget", "The standard epoch is JD 2451545.0, i.e. 2000 January 1 12 hours TDB.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SensorInfoWidget: public Ui_SensorInfoWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SENSORINFOWIDGET_H
