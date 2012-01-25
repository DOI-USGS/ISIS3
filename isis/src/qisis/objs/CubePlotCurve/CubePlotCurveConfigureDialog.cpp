#include "CubePlotCurveConfigureDialog.h"

#include <iostream>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "CubePlotCurve.h"

namespace Isis {
  /**
   * This instantiates a configuration dialog associated with the given cube
   *   plot curve.
   *
   * @param curve The plot curve to be configured
   * @param parent The parent widget/widget who owns this dialog.
   */
  CubePlotCurveConfigureDialog::CubePlotCurveConfigureDialog(
      CubePlotCurve *curve, QWidget *parent) : QDialog(parent) {
    m_plotCurve = curve;

    QGridLayout *optionsLayout = new QGridLayout;

    int row = 0;
    QLabel *titleLabel = new QLabel("Curve Name: ");
    m_nameEdit = new QLineEdit(curve->title().text());
    optionsLayout->addWidget(titleLabel, row, 0);
    optionsLayout->addWidget(m_nameEdit,  row, 1);
    row++;

    QLabel *colorLabel = new QLabel("Color: ");
    m_colorButton = new QPushButton;
    m_colorButton->setFixedWidth(25);
    connect(m_colorButton, SIGNAL(clicked()), this, SLOT(askUserForColor()));
    optionsLayout->addWidget(colorLabel,    row, 0);
    optionsLayout->addWidget(m_colorButton, row, 1);
    row++;

    QLabel *styleLabel = new QLabel("Style:");
    m_styleCombo = new QComboBox;
    m_styleCombo->addItem("No Line", Qt::NoPen);
    m_styleCombo->addItem("Solid Line", Qt::SolidLine);
    m_styleCombo->addItem("Dash Line", Qt::DashLine);
    m_styleCombo->addItem("Dot Line", Qt::DotLine);
    m_styleCombo->addItem("Dash Dot Line", Qt::DashDotLine);
    m_styleCombo->addItem("Dash Dot Dot Line", Qt::DashDotDotLine);
    optionsLayout->addWidget(styleLabel,   row, 0);
    optionsLayout->addWidget(m_styleCombo, row, 1);
    row++;

    QLabel *sizeLabel = new QLabel("Size:");
    m_sizeCombo = new QComboBox;
    m_sizeCombo->addItem("1", 1);
    m_sizeCombo->addItem("2", 2);
    m_sizeCombo->addItem("3", 3);
    m_sizeCombo->addItem("4", 4);
    optionsLayout->addWidget(sizeLabel, row, 0);
    optionsLayout->addWidget(m_sizeCombo, row, 1);
    row++;

    QLabel *symbolLabel = new QLabel("Symbol:");
    m_symbolCombo = new QComboBox;
    m_symbolCombo->addItem("None", QwtSymbol::NoSymbol);

    m_symbolCombo->addItem("Diamond", QwtSymbol::Diamond);
    m_symbolCombo->addItem("Rectangle", QwtSymbol::Rect);
    m_symbolCombo->addItem("Triangle", QwtSymbol::Triangle);
    m_symbolCombo->insertSeparator(m_symbolCombo->count());

    m_symbolCombo->addItem("Down Facing Triangle", QwtSymbol::UTriangle);
    m_symbolCombo->addItem("Up Facing Triangle", QwtSymbol::DTriangle);
    m_symbolCombo->addItem("Left Facing Triangle", QwtSymbol::RTriangle);
    m_symbolCombo->addItem("Right Facing Triangle", QwtSymbol::LTriangle);
    m_symbolCombo->insertSeparator(m_symbolCombo->count());

    m_symbolCombo->addItem("Diagonal Cross (X)", QwtSymbol::XCross);
    m_symbolCombo->addItem("Eight-Pointed Star", QwtSymbol::Star1);
    m_symbolCombo->addItem("Ellipse", QwtSymbol::Ellipse);
    m_symbolCombo->addItem("Hexagon", QwtSymbol::Hexagon);
    m_symbolCombo->addItem("Horizontal Line", QwtSymbol::HLine);
    m_symbolCombo->addItem("Plus Sign (+)", QwtSymbol::Cross);
    m_symbolCombo->addItem("Six-Pointed Star", QwtSymbol::Star2);
    m_symbolCombo->addItem("Vertical Line", QwtSymbol::VLine);
    optionsLayout->addWidget(symbolLabel, row, 0);
    optionsLayout->addWidget(m_symbolCombo, row, 1);
    row++;

    QHBoxLayout *applyButtonsLayout = new QHBoxLayout;
    applyButtonsLayout->addStretch();

    QPushButton *okay = new QPushButton("&Ok");
    okay->setIcon(QIcon::fromTheme("dialog-ok"));
    connect(okay, SIGNAL(clicked()),
            this, SLOT(applySettingsToCurve()));
    connect(okay, SIGNAL(clicked()),
            this, SLOT(close()));
    applyButtonsLayout->addWidget(okay);

    QPushButton *apply = new QPushButton("&Apply");
    apply->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    connect(apply, SIGNAL(clicked()),
            this, SLOT(applySettingsToCurve()));
    applyButtonsLayout->addWidget(apply);

    QPushButton *cancel = new QPushButton("&Cancel");
    cancel->setIcon(QIcon::fromTheme("dialog-cancel"));
    connect(cancel, SIGNAL(clicked()),
            this, SLOT(close()));
    applyButtonsLayout->addWidget(cancel);

    QWidget *optionsHolder = new QWidget;
    optionsHolder->setLayout(optionsLayout);

    QWidget *applyButtonsHolder = new QWidget;
    applyButtonsHolder->setLayout(applyButtonsLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(optionsHolder);
    mainLayout->addWidget(applyButtonsHolder);

    setLayout(mainLayout);

    readSettingsFromCurve();
  }


  /**
   * This destroys the configuration dialog, which happens when the user closes
   *   it or clicks ok or cancel.
   */
  CubePlotCurveConfigureDialog::~CubePlotCurveConfigureDialog() {
    m_colorButton = NULL;
    m_plotCurve = NULL;
  }


  /**
   * This takes the configuration settings and applies them to the plot curve.
   *   This happens when the user clicks 'apply' or 'ok.' Any settings that fail
   *   to be applied correctly will be reverted in the GUI.
   */
  void CubePlotCurveConfigureDialog::applySettingsToCurve() {
    if (m_plotCurve->title() != m_nameEdit->text()) {
      m_plotCurve->enableAutoRenaming(false);
      m_plotCurve->setTitle(m_nameEdit->text());
    }

    QPalette colorPalette(m_colorButton->palette());

    QPen curvePen;
    curvePen.setColor(colorPalette.color(QPalette::Button));

    int penWidth = m_sizeCombo->itemData(m_sizeCombo->currentIndex()).toInt();
    curvePen.setWidth(penWidth);

    Qt::PenStyle penStyle = (Qt::PenStyle)m_styleCombo->itemData(
        m_styleCombo->currentIndex()).toInt();
    curvePen.setStyle(penStyle);

    m_plotCurve->setPen(curvePen);
    m_plotCurve->setColor(colorPalette.color(QPalette::Button));


    QwtSymbol::Style symbolStyle = (QwtSymbol::Style)m_symbolCombo->itemData(
        m_symbolCombo->currentIndex()).toInt();
    QwtSymbol newSymbol(m_plotCurve->markerSymbol());
    newSymbol.setStyle(symbolStyle);
    m_plotCurve->setMarkerSymbol(newSymbol);

    m_plotCurve->show();
    m_plotCurve->plot()->replot();

    // Put us in a state as if we just opened the config dialog
    readSettingsFromCurve();
  }


  /**
   * This takes the current data inside of the plot curve and populates this
   *   configuration dialog's widgets with the appropriate data/settings.
   */
  void CubePlotCurveConfigureDialog::readSettingsFromCurve() {
    setWindowTitle("Configure " + m_plotCurve->title().text());

    m_nameEdit->setText(m_plotCurve->title().text());

    QPalette colorPalette;
    colorPalette.setColor(QPalette::Button, m_plotCurve->pen().color());
    m_colorButton->setPalette(colorPalette);

    if (m_sizeCombo->count()) {
      m_sizeCombo->setCurrentIndex(0);
      for (int i = 0; i < m_sizeCombo->count(); i++) {
        if (m_sizeCombo->itemData(i) == m_plotCurve->pen().width())
          m_sizeCombo->setCurrentIndex(i);
      }
    }

    if (m_styleCombo->count()) {
      m_styleCombo->setCurrentIndex(0);
      for (int i = 0; i < m_styleCombo->count(); i++) {
        if (m_styleCombo->itemData(i) == m_plotCurve->pen().style())
          m_styleCombo->setCurrentIndex(i);
      }
    }

    if (m_symbolCombo->count()) {
      m_symbolCombo->setCurrentIndex(0);
      for (int i = 0; i < m_symbolCombo->count(); i++) {
        if (m_symbolCombo->itemData(i).toInt() ==
            m_plotCurve->markerSymbol().style()) {
          m_symbolCombo->setCurrentIndex(i);
        }
      }
    }
  }


  /**
   * This prompts the user to select a new color for the curve/curve markers.
   */
  void CubePlotCurveConfigureDialog::askUserForColor() {
    QPalette colorPalette(m_colorButton->palette());

    QColor newColor = QColorDialog::getColor(
        colorPalette.color(QPalette::Button), this);

    if(newColor.isValid()) {
      colorPalette.setColor(QPalette::Button, newColor);
      m_colorButton->setPalette(colorPalette);
    }
  }
}
