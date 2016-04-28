#include "NomenclatureToolConfigDialog.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "FeatureNomenclatureTool.h"

namespace Isis {
  /**
   * Create a config dialog that configures the given FeatureNomenclatureTool.
   *
   * @param tool The tool to read settings from and write settings to.
   * @param parent The qt-parent relationship parent.
   */
  NomenclatureToolConfigDialog::NomenclatureToolConfigDialog(
      FeatureNomenclatureTool *tool, QWidget *parent) : QDialog(parent) {
    m_tool = tool;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QWidget *settingsAreaWidget = new QWidget;
    mainLayout->addWidget(settingsAreaWidget);

    QWidget *buttonsAreaWidget = new QWidget;
    mainLayout->addWidget(buttonsAreaWidget);

    QGridLayout *settingsAreaLayout = new QGridLayout;
    settingsAreaWidget->setLayout(settingsAreaLayout);

    // Settings area
    int row = 0;
    QLabel *fontSizeLabel = new QLabel("Font Size");
    settingsAreaLayout->addWidget(fontSizeLabel, row, 0);

    m_fontSizeCombo = new QComboBox;
    for (int i = 8; i <= 20; i++) {
      m_fontSizeCombo->addItem(QString::number(i), i);
    }
    settingsAreaLayout->addWidget(m_fontSizeCombo, row, 1);
    row++;

    QLabel *fontColorLabel = new QLabel("Font Color");
    settingsAreaLayout->addWidget(fontColorLabel, row, 0);

    m_fontColorButton = new QPushButton;
    settingsAreaLayout->addWidget(m_fontColorButton, row, 1);
    connect(m_fontColorButton, SIGNAL(clicked()),
            this, SLOT(askUserForColor()));
    row++;

    QLabel *showVectorsLabel = new QLabel("Show feature extents");
    settingsAreaLayout->addWidget(showVectorsLabel, row, 0);

    m_showVectorsCombo = new QComboBox;

    m_showVectorsCombo->addItem("None", FeatureNomenclatureTool::None);
    m_showVectorsCombo->addItem("4 Arrows", FeatureNomenclatureTool::Arrows4);
    m_showVectorsCombo->addItem("8 Arrows", FeatureNomenclatureTool::Arrows8);
    m_showVectorsCombo->addItem("Box", FeatureNomenclatureTool::Box);
    
    settingsAreaLayout->addWidget(m_showVectorsCombo, row, 1);
    row++;

    QLabel *showApprovedLabel = new QLabel("Show IAU approved only");
    settingsAreaLayout->addWidget(showApprovedLabel, row, 0);

    m_showApprovedCheckBox = new QCheckBox;
    settingsAreaLayout->addWidget(m_showApprovedCheckBox, row, 1);
    row++;

    QLabel *defaultOnLabel = new QLabel(
        "Enabled when " + QCoreApplication::instance()->applicationName() +
        " starts");
    settingsAreaLayout->addWidget(defaultOnLabel, row, 0);

    m_defaultOnCheckBox = new QCheckBox;
    settingsAreaLayout->addWidget(m_defaultOnCheckBox, row, 1);
    row++;

    // Now the buttons area
    QHBoxLayout *buttonsAreaLayout = new QHBoxLayout;
    buttonsAreaWidget->setLayout(buttonsAreaLayout);

    buttonsAreaLayout->addStretch();
    
    QPushButton *okayButton = new QPushButton("&Ok");
    okayButton->setIcon(QIcon::fromTheme("dialog-ok"));
    buttonsAreaLayout->addWidget(okayButton);
    connect(okayButton, SIGNAL(clicked()),
            this, SLOT(applySettings()));
    connect(okayButton, SIGNAL(clicked()),
            this, SLOT(accept()));

    QPushButton *applyButton = new QPushButton("&Apply");
    applyButton->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    buttonsAreaLayout->addWidget(applyButton);
    connect(applyButton, SIGNAL(clicked()),
            this, SLOT(applySettings()));

    QPushButton *cancelButton = new QPushButton("&Cancel");
    cancelButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    buttonsAreaLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    readSettings();
  }

  /**
   * Clean up allocated memory.
   */
  NomenclatureToolConfigDialog::~NomenclatureToolConfigDialog() {

  }


  /**
   * Apply the user's current settings to the tool.
   */
  void NomenclatureToolConfigDialog::applySettings() {
    m_tool->setFontSize(
        m_fontSizeCombo->itemData(m_fontSizeCombo->currentIndex()).toInt());

    QPalette colorPalette(m_fontColorButton->palette());
    m_tool->setFontColor(colorPalette.color(QPalette::Button));

    m_tool->setDefaultEnabled(m_defaultOnCheckBox->isChecked());

    m_tool->setShowApprovedOnly(m_showApprovedCheckBox->isChecked());

    m_tool->setVectorType( (FeatureNomenclatureTool::VectorType)
        m_showVectorsCombo->itemData(m_showVectorsCombo->currentIndex()).toInt() );

    readSettings();
  }


  /**
   * Read the tool's current settings and set the widget states to match.
   */
  void NomenclatureToolConfigDialog::readSettings() {
    m_fontSizeCombo->setCurrentIndex(
      m_fontSizeCombo->findText(QString::number(m_tool->fontSize())));

    QPalette colorPalette;
    colorPalette.setColor(QPalette::Button, m_tool->fontColor());
    m_fontColorButton->setPalette(colorPalette);

    m_defaultOnCheckBox->setChecked(m_tool->defaultEnabled());
    
    m_showApprovedCheckBox->setChecked(m_tool->showApprovedOnly());

    m_showVectorsCombo->setCurrentIndex(
      m_showVectorsCombo->findData(m_tool->vectorType()));
  }


  /**
   * Prompt the user for a new font color.
   */
  void NomenclatureToolConfigDialog::askUserForColor() {
    QPalette colorPalette(m_fontColorButton->palette());

    QColor newColor = QColorDialog::getColor(
        colorPalette.color(QPalette::Button), this);

    if(newColor.isValid()) {
      colorPalette.setColor(QPalette::Button, newColor);
      m_fontColorButton->setPalette(colorPalette);
    }
  }
}