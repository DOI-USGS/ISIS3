/********************************************************************************
** Form generated from reading UI file 'optionsWidget.ui'
**
** Created: Tue Jul 15 09:13:38 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONSWIDGET_H
#define UI_OPTIONSWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "kcolorbutton.h"

QT_BEGIN_NAMESPACE

class Ui_matrixOptionsWidget
{
public:
    QTabWidget *matrixOptionsTabs;
    QWidget *colorOptionsPage;
    QGridLayout *mainLayout;
    QHBoxLayout *colorPageLayout;
    QVBoxLayout *toleranceLayout;
    QRadioButton *toleranceRadioButton;
    QHBoxLayout *toleranceOptionsLayout;
    QSlider *toleranceSlider;
    QVBoxLayout *toleranceEditsLayout;
    QHBoxLayout *badCorrelationLayout;
    QLabel *badCorrelationLabel;
    KColorButton *badCorrelationColorButton;
    QSpacerItem *badTolSpacer;
    QHBoxLayout *enterToleranceLayout;
    QLabel *toleranceLabel;
    QLineEdit *toleranceLineEdit;
    QSpacerItem *tolGoodSpacer;
    QHBoxLayout *goodCorrelationLayout;
    QLabel *goodCorrelationLabel;
    KColorButton *goodCorrelationColorButton;
    QVBoxLayout *gradientLayout;
    QRadioButton *gradientRadioButton;
    QSpacerItem *gradientSpacer;
    QWidget *focusOptionsPage;
    QWidget *horizontalLayoutWidget_11;
    QHBoxLayout *focusPageLayout;
    QHBoxLayout *focusPageLayout_2;
    QVBoxLayout *focusOptionsLayout;
    QRadioButton *bestCorrelationRadioButton;
    QRadioButton *worstCorrelationRadioButton;
    QVBoxLayout *specificParametersLayout;
    QRadioButton *specificCorrelationRadioButton;
    QLabel *focusParametersLabel1;
    QHBoxLayout *specificParameter1Layout;
    QComboBox *image1ComboBox;
    QComboBox *parameter1ComboBox;
    QLabel *focusParametersLabel2;
    QHBoxLayout *specificPrameter2Layout;
    QComboBox *image2ComboBox;
    QComboBox *parameter2ComboBox;
    QVBoxLayout *focusToleranceLayout;
    QRadioButton *focusToleranceRadioButton;
    QHBoxLayout *toleranceEditLayout;
    QLabel *focusToleranceLabel;
    QLineEdit *focusToleranceLineEdit;
    QHBoxLayout *elementsLayout;
    QVBoxLayout *goodElementsLayout;
    QLabel *goodElementsLabel;
    QSpinBox *goodElementsSpinBox;
    QVBoxLayout *badElementsLayout;
    QLabel *badElementsLabel;
    QSpinBox *badElementsSpinBox;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *currentElementData;
    QHBoxLayout *correlationLayout;
    QLabel *correlationLabel;
    QLabel *currentValue;
    QHBoxLayout *image1Layout;
    QLabel *image1Name;
    QLabel *parameter1Name;
    QHBoxLayout *image2Layout;
    QLabel *image2Name;
    QLabel *parameter2Name;

    void setupUi(QWidget *matrixOptionsWidget)
    {
        if (matrixOptionsWidget->objectName().isEmpty())
            matrixOptionsWidget->setObjectName(QString::fromUtf8("matrixOptionsWidget"));
        matrixOptionsWidget->resize(597, 549);
        matrixOptionsTabs = new QTabWidget(matrixOptionsWidget);
        matrixOptionsTabs->setObjectName(QString::fromUtf8("matrixOptionsTabs"));
        matrixOptionsTabs->setGeometry(QRect(9, 9, 581, 391));
        colorOptionsPage = new QWidget();
        colorOptionsPage->setObjectName(QString::fromUtf8("colorOptionsPage"));
        mainLayout = new QGridLayout(colorOptionsPage);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        colorPageLayout = new QHBoxLayout();
        colorPageLayout->setObjectName(QString::fromUtf8("colorPageLayout"));
        toleranceLayout = new QVBoxLayout();
        toleranceLayout->setObjectName(QString::fromUtf8("toleranceLayout"));
        toleranceRadioButton = new QRadioButton(colorOptionsPage);
        toleranceRadioButton->setObjectName(QString::fromUtf8("toleranceRadioButton"));

        toleranceLayout->addWidget(toleranceRadioButton);

        toleranceOptionsLayout = new QHBoxLayout();
        toleranceOptionsLayout->setObjectName(QString::fromUtf8("toleranceOptionsLayout"));
        toleranceSlider = new QSlider(colorOptionsPage);
        toleranceSlider->setObjectName(QString::fromUtf8("toleranceSlider"));
        toleranceSlider->setOrientation(Qt::Vertical);

        toleranceOptionsLayout->addWidget(toleranceSlider);

        toleranceEditsLayout = new QVBoxLayout();
        toleranceEditsLayout->setObjectName(QString::fromUtf8("toleranceEditsLayout"));
        badCorrelationLayout = new QHBoxLayout();
        badCorrelationLayout->setObjectName(QString::fromUtf8("badCorrelationLayout"));
        badCorrelationLabel = new QLabel(colorOptionsPage);
        badCorrelationLabel->setObjectName(QString::fromUtf8("badCorrelationLabel"));

        badCorrelationLayout->addWidget(badCorrelationLabel);

        badCorrelationColorButton = new KColorButton(colorOptionsPage);
        badCorrelationColorButton->setObjectName(QString::fromUtf8("badCorrelationColorButton"));

        badCorrelationLayout->addWidget(badCorrelationColorButton);


        toleranceEditsLayout->addLayout(badCorrelationLayout);

        badTolSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        toleranceEditsLayout->addItem(badTolSpacer);

        enterToleranceLayout = new QHBoxLayout();
        enterToleranceLayout->setObjectName(QString::fromUtf8("enterToleranceLayout"));
        toleranceLabel = new QLabel(colorOptionsPage);
        toleranceLabel->setObjectName(QString::fromUtf8("toleranceLabel"));

        enterToleranceLayout->addWidget(toleranceLabel);

        toleranceLineEdit = new QLineEdit(colorOptionsPage);
        toleranceLineEdit->setObjectName(QString::fromUtf8("toleranceLineEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toleranceLineEdit->sizePolicy().hasHeightForWidth());
        toleranceLineEdit->setSizePolicy(sizePolicy);
        toleranceLineEdit->setMaximumSize(QSize(100, 30));

        enterToleranceLayout->addWidget(toleranceLineEdit);


        toleranceEditsLayout->addLayout(enterToleranceLayout);

        tolGoodSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        toleranceEditsLayout->addItem(tolGoodSpacer);

        goodCorrelationLayout = new QHBoxLayout();
        goodCorrelationLayout->setObjectName(QString::fromUtf8("goodCorrelationLayout"));
        goodCorrelationLabel = new QLabel(colorOptionsPage);
        goodCorrelationLabel->setObjectName(QString::fromUtf8("goodCorrelationLabel"));

        goodCorrelationLayout->addWidget(goodCorrelationLabel);

        goodCorrelationColorButton = new KColorButton(colorOptionsPage);
        goodCorrelationColorButton->setObjectName(QString::fromUtf8("goodCorrelationColorButton"));

        goodCorrelationLayout->addWidget(goodCorrelationColorButton);


        toleranceEditsLayout->addLayout(goodCorrelationLayout);


        toleranceOptionsLayout->addLayout(toleranceEditsLayout);


        toleranceLayout->addLayout(toleranceOptionsLayout);


        colorPageLayout->addLayout(toleranceLayout);

        gradientLayout = new QVBoxLayout();
        gradientLayout->setObjectName(QString::fromUtf8("gradientLayout"));
        gradientRadioButton = new QRadioButton(colorOptionsPage);
        gradientRadioButton->setObjectName(QString::fromUtf8("gradientRadioButton"));

        gradientLayout->addWidget(gradientRadioButton);

        gradientSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gradientLayout->addItem(gradientSpacer);


        colorPageLayout->addLayout(gradientLayout);


        mainLayout->addLayout(colorPageLayout, 0, 1, 1, 1);

        matrixOptionsTabs->addTab(colorOptionsPage, QString());
        focusOptionsPage = new QWidget();
        focusOptionsPage->setObjectName(QString::fromUtf8("focusOptionsPage"));
        horizontalLayoutWidget_11 = new QWidget(focusOptionsPage);
        horizontalLayoutWidget_11->setObjectName(QString::fromUtf8("horizontalLayoutWidget_11"));
        horizontalLayoutWidget_11->setGeometry(QRect(0, 0, 577, 351));
        focusPageLayout = new QHBoxLayout(horizontalLayoutWidget_11);
        focusPageLayout->setObjectName(QString::fromUtf8("focusPageLayout"));
        focusPageLayout->setContentsMargins(0, 0, 0, 0);
        focusPageLayout_2 = new QHBoxLayout();
        focusPageLayout_2->setObjectName(QString::fromUtf8("focusPageLayout_2"));
        focusOptionsLayout = new QVBoxLayout();
        focusOptionsLayout->setObjectName(QString::fromUtf8("focusOptionsLayout"));
        bestCorrelationRadioButton = new QRadioButton(horizontalLayoutWidget_11);
        bestCorrelationRadioButton->setObjectName(QString::fromUtf8("bestCorrelationRadioButton"));

        focusOptionsLayout->addWidget(bestCorrelationRadioButton);

        worstCorrelationRadioButton = new QRadioButton(horizontalLayoutWidget_11);
        worstCorrelationRadioButton->setObjectName(QString::fromUtf8("worstCorrelationRadioButton"));

        focusOptionsLayout->addWidget(worstCorrelationRadioButton);

        specificParametersLayout = new QVBoxLayout();
        specificParametersLayout->setObjectName(QString::fromUtf8("specificParametersLayout"));
        specificCorrelationRadioButton = new QRadioButton(horizontalLayoutWidget_11);
        specificCorrelationRadioButton->setObjectName(QString::fromUtf8("specificCorrelationRadioButton"));

        specificParametersLayout->addWidget(specificCorrelationRadioButton);

        focusParametersLabel1 = new QLabel(horizontalLayoutWidget_11);
        focusParametersLabel1->setObjectName(QString::fromUtf8("focusParametersLabel1"));
        focusParametersLabel1->setMaximumSize(QSize(16777215, 30));

        specificParametersLayout->addWidget(focusParametersLabel1);

        specificParameter1Layout = new QHBoxLayout();
        specificParameter1Layout->setObjectName(QString::fromUtf8("specificParameter1Layout"));
        image1ComboBox = new QComboBox(horizontalLayoutWidget_11);
        image1ComboBox->setObjectName(QString::fromUtf8("image1ComboBox"));

        specificParameter1Layout->addWidget(image1ComboBox);

        parameter1ComboBox = new QComboBox(horizontalLayoutWidget_11);
        parameter1ComboBox->setObjectName(QString::fromUtf8("parameter1ComboBox"));

        specificParameter1Layout->addWidget(parameter1ComboBox);


        specificParametersLayout->addLayout(specificParameter1Layout);

        focusParametersLabel2 = new QLabel(horizontalLayoutWidget_11);
        focusParametersLabel2->setObjectName(QString::fromUtf8("focusParametersLabel2"));
        focusParametersLabel2->setMaximumSize(QSize(16777215, 30));

        specificParametersLayout->addWidget(focusParametersLabel2);

        specificPrameter2Layout = new QHBoxLayout();
        specificPrameter2Layout->setObjectName(QString::fromUtf8("specificPrameter2Layout"));
        image2ComboBox = new QComboBox(horizontalLayoutWidget_11);
        image2ComboBox->setObjectName(QString::fromUtf8("image2ComboBox"));

        specificPrameter2Layout->addWidget(image2ComboBox);

        parameter2ComboBox = new QComboBox(horizontalLayoutWidget_11);
        parameter2ComboBox->setObjectName(QString::fromUtf8("parameter2ComboBox"));

        specificPrameter2Layout->addWidget(parameter2ComboBox);


        specificParametersLayout->addLayout(specificPrameter2Layout);


        focusOptionsLayout->addLayout(specificParametersLayout);


        focusPageLayout_2->addLayout(focusOptionsLayout);

        focusToleranceLayout = new QVBoxLayout();
        focusToleranceLayout->setObjectName(QString::fromUtf8("focusToleranceLayout"));
        focusToleranceRadioButton = new QRadioButton(horizontalLayoutWidget_11);
        focusToleranceRadioButton->setObjectName(QString::fromUtf8("focusToleranceRadioButton"));

        focusToleranceLayout->addWidget(focusToleranceRadioButton);

        toleranceEditLayout = new QHBoxLayout();
        toleranceEditLayout->setObjectName(QString::fromUtf8("toleranceEditLayout"));
        focusToleranceLabel = new QLabel(horizontalLayoutWidget_11);
        focusToleranceLabel->setObjectName(QString::fromUtf8("focusToleranceLabel"));
        focusToleranceLabel->setMaximumSize(QSize(16777215, 30));

        toleranceEditLayout->addWidget(focusToleranceLabel);

        focusToleranceLineEdit = new QLineEdit(horizontalLayoutWidget_11);
        focusToleranceLineEdit->setObjectName(QString::fromUtf8("focusToleranceLineEdit"));

        toleranceEditLayout->addWidget(focusToleranceLineEdit);


        focusToleranceLayout->addLayout(toleranceEditLayout);

        elementsLayout = new QHBoxLayout();
        elementsLayout->setObjectName(QString::fromUtf8("elementsLayout"));
        goodElementsLayout = new QVBoxLayout();
        goodElementsLayout->setObjectName(QString::fromUtf8("goodElementsLayout"));
        goodElementsLabel = new QLabel(horizontalLayoutWidget_11);
        goodElementsLabel->setObjectName(QString::fromUtf8("goodElementsLabel"));
        goodElementsLabel->setMaximumSize(QSize(16777215, 30));
        goodElementsLabel->setFocusPolicy(Qt::NoFocus);

        goodElementsLayout->addWidget(goodElementsLabel);

        goodElementsSpinBox = new QSpinBox(horizontalLayoutWidget_11);
        goodElementsSpinBox->setObjectName(QString::fromUtf8("goodElementsSpinBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(goodElementsSpinBox->sizePolicy().hasHeightForWidth());
        goodElementsSpinBox->setSizePolicy(sizePolicy1);
        goodElementsSpinBox->setMaximumSize(QSize(16777215, 900));

        goodElementsLayout->addWidget(goodElementsSpinBox);


        elementsLayout->addLayout(goodElementsLayout);

        badElementsLayout = new QVBoxLayout();
        badElementsLayout->setObjectName(QString::fromUtf8("badElementsLayout"));
        badElementsLabel = new QLabel(horizontalLayoutWidget_11);
        badElementsLabel->setObjectName(QString::fromUtf8("badElementsLabel"));
        badElementsLabel->setMaximumSize(QSize(16777215, 30));

        badElementsLayout->addWidget(badElementsLabel);

        badElementsSpinBox = new QSpinBox(horizontalLayoutWidget_11);
        badElementsSpinBox->setObjectName(QString::fromUtf8("badElementsSpinBox"));
        badElementsSpinBox->setMaximumSize(QSize(16777215, 600));

        badElementsLayout->addWidget(badElementsSpinBox);


        elementsLayout->addLayout(badElementsLayout);


        focusToleranceLayout->addLayout(elementsLayout);


        focusPageLayout_2->addLayout(focusToleranceLayout);


        focusPageLayout->addLayout(focusPageLayout_2);

        matrixOptionsTabs->addTab(focusOptionsPage, QString());
        verticalLayoutWidget = new QWidget(matrixOptionsWidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 410, 581, 131));
        currentElementData = new QVBoxLayout(verticalLayoutWidget);
        currentElementData->setObjectName(QString::fromUtf8("currentElementData"));
        currentElementData->setContentsMargins(0, 0, 0, 0);
        correlationLayout = new QHBoxLayout();
        correlationLayout->setObjectName(QString::fromUtf8("correlationLayout"));
        correlationLabel = new QLabel(verticalLayoutWidget);
        correlationLabel->setObjectName(QString::fromUtf8("correlationLabel"));

        correlationLayout->addWidget(correlationLabel);

        currentValue = new QLabel(verticalLayoutWidget);
        currentValue->setObjectName(QString::fromUtf8("currentValue"));

        correlationLayout->addWidget(currentValue);


        currentElementData->addLayout(correlationLayout);

        image1Layout = new QHBoxLayout();
        image1Layout->setObjectName(QString::fromUtf8("image1Layout"));
        image1Name = new QLabel(verticalLayoutWidget);
        image1Name->setObjectName(QString::fromUtf8("image1Name"));

        image1Layout->addWidget(image1Name);

        parameter1Name = new QLabel(verticalLayoutWidget);
        parameter1Name->setObjectName(QString::fromUtf8("parameter1Name"));

        image1Layout->addWidget(parameter1Name);


        currentElementData->addLayout(image1Layout);

        image2Layout = new QHBoxLayout();
        image2Layout->setObjectName(QString::fromUtf8("image2Layout"));
        image2Name = new QLabel(verticalLayoutWidget);
        image2Name->setObjectName(QString::fromUtf8("image2Name"));

        image2Layout->addWidget(image2Name);

        parameter2Name = new QLabel(verticalLayoutWidget);
        parameter2Name->setObjectName(QString::fromUtf8("parameter2Name"));

        image2Layout->addWidget(parameter2Name);


        currentElementData->addLayout(image2Layout);


        retranslateUi(matrixOptionsWidget);

        matrixOptionsTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(matrixOptionsWidget);
    } // setupUi

    void retranslateUi(QWidget *matrixOptionsWidget)
    {
        matrixOptionsWidget->setWindowTitle(QApplication::translate("matrixOptionsWidget", "Matrix Options", 0, QApplication::UnicodeUTF8));
        toleranceRadioButton->setText(QApplication::translate("matrixOptionsWidget", "Pick Tolerance", 0, QApplication::UnicodeUTF8));
        badCorrelationLabel->setText(QApplication::translate("matrixOptionsWidget", "Bad Correlation", 0, QApplication::UnicodeUTF8));
        toleranceLabel->setText(QApplication::translate("matrixOptionsWidget", "Tolerance:", 0, QApplication::UnicodeUTF8));
        goodCorrelationLabel->setText(QApplication::translate("matrixOptionsWidget", "Good Correlation", 0, QApplication::UnicodeUTF8));
        gradientRadioButton->setText(QApplication::translate("matrixOptionsWidget", "Color Gradient", 0, QApplication::UnicodeUTF8));
        matrixOptionsTabs->setTabText(matrixOptionsTabs->indexOf(colorOptionsPage), QApplication::translate("matrixOptionsWidget", "Color Options", 0, QApplication::UnicodeUTF8));
        bestCorrelationRadioButton->setText(QApplication::translate("matrixOptionsWidget", "Best Correlation", 0, QApplication::UnicodeUTF8));
        worstCorrelationRadioButton->setText(QApplication::translate("matrixOptionsWidget", "Worst Correlation", 0, QApplication::UnicodeUTF8));
        specificCorrelationRadioButton->setText(QApplication::translate("matrixOptionsWidget", "Specific Parameters", 0, QApplication::UnicodeUTF8));
        focusParametersLabel1->setText(QApplication::translate("matrixOptionsWidget", "Image 1:", 0, QApplication::UnicodeUTF8));
        focusParametersLabel2->setText(QApplication::translate("matrixOptionsWidget", "Image 2:", 0, QApplication::UnicodeUTF8));
        focusToleranceRadioButton->setText(QApplication::translate("matrixOptionsWidget", "Tolerance", 0, QApplication::UnicodeUTF8));
        focusToleranceLabel->setText(QApplication::translate("matrixOptionsWidget", "Tolerance", 0, QApplication::UnicodeUTF8));
        goodElementsLabel->setText(QApplication::translate("matrixOptionsWidget", "Good Correlations", 0, QApplication::UnicodeUTF8));
        badElementsLabel->setText(QApplication::translate("matrixOptionsWidget", "Bad Correlations", 0, QApplication::UnicodeUTF8));
        matrixOptionsTabs->setTabText(matrixOptionsTabs->indexOf(focusOptionsPage), QApplication::translate("matrixOptionsWidget", "Focus Options", 0, QApplication::UnicodeUTF8));
        correlationLabel->setText(QApplication::translate("matrixOptionsWidget", "Current Correlation Value:", 0, QApplication::UnicodeUTF8));
        currentValue->setText(QApplication::translate("matrixOptionsWidget", "Current Value", 0, QApplication::UnicodeUTF8));
        image1Name->setText(QApplication::translate("matrixOptionsWidget", "Image 1 Name", 0, QApplication::UnicodeUTF8));
        parameter1Name->setText(QApplication::translate("matrixOptionsWidget", "Parameter 1 Name", 0, QApplication::UnicodeUTF8));
        image2Name->setText(QApplication::translate("matrixOptionsWidget", "Image 2 Name", 0, QApplication::UnicodeUTF8));
        parameter2Name->setText(QApplication::translate("matrixOptionsWidget", "Parameter 2 Name", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class matrixOptionsWidget: public Ui_matrixOptionsWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONSWIDGET_H
