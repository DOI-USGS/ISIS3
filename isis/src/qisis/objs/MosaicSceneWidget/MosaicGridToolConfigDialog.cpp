#include "MosaicGridToolConfigDialog.h"

#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QtGui>
#include <QElapsedTimer>

#include "Angle.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGridTool.h"
#include "MosaicGridToolConfigDialog.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"
#include "PvlGroup.h"

namespace Isis {
  /**
   * Create a config dialog that configures the given MosaicGridTool.
   *
   * @param tool The tool to read settings from and write settings to.
   * @param parent The qt-parent relationship parent.
   */
  MosaicGridToolConfigDialog::MosaicGridToolConfigDialog(
      MosaicGridTool *tool, QWidget *parent) : QDialog(parent) {

   /*
    * What's This' provided here were written by Tammy Becker (2011-12-05) with
    *   help from Steven Lambright.
    */
    m_tool = tool;

    setWindowTitle("Grid Options");

    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    int row = 0;

    QString showGridWhatsThis =
        "Check or uncheck to draw or clear the grid.";
    QLabel *showGridLabel = new QLabel("&Show Grid");
    showGridLabel->setWhatsThis(showGridWhatsThis);
    mainLayout->addWidget(showGridLabel, row, 0, 1, 2);

    m_showGridCheckBox = new QCheckBox;
    showGridLabel->setBuddy(m_showGridCheckBox);
    m_showGridCheckBox->setWhatsThis(showGridWhatsThis);
    connect(m_showGridCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_showGridCheckBox, row, 3, 1, 2, Qt::AlignRight);
    row++;

    QString autoGridWhatsThis =
        "Draws a grid based on the current lat/lon extents (from the cubes, map, or user).";
    QLabel *autoGridLabel = new QLabel("Auto &Grid");
    autoGridLabel->setWhatsThis(autoGridWhatsThis);
    mainLayout->addWidget(autoGridLabel, row, 0, 1, 2);
    
    m_autoGridCheckBox = new QCheckBox;
    autoGridLabel->setBuddy(m_autoGridCheckBox);
    m_autoGridCheckBox->setWhatsThis(autoGridWhatsThis);
    connect(m_autoGridCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_autoGridCheckBox, row, 3, 1, 2, Qt::AlignRight);
    row++;

    QString baseLatWhatsThis =
        "The origin for the first latitude line. The first line of the grid "
        "will be drawn at the base latitude. Successive latitude lines will "
        "then be drawn relative to base latitude at an increment defined by "
        "the latitude increment. Base latitude can be outside the range of "
        "the image data.";
    m_baseLatLabel = new QLabel("Base Latitude");
    m_baseLatLabel->setWhatsThis(baseLatWhatsThis);
    mainLayout->addWidget(m_baseLatLabel, row, 0, 1, 2);

    m_baseLatSlider = new QSlider(Qt::Horizontal);
    m_baseLatSlider->setRange(0, 100);
    m_baseLatSlider->setWhatsThis(baseLatWhatsThis);
    connect(m_baseLatSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onBaseLatSliderChanged()));
    mainLayout->addWidget(m_baseLatSlider, row, 2, 1, 1);
    
    m_baseLatLineEdit = new QLineEdit("0");
    m_baseLatLineEdit->setValidator(new QDoubleValidator(-90.0, 90.0, 99, this));
    m_baseLatLineEdit->setWhatsThis(baseLatWhatsThis);
    connect(m_baseLatLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_baseLatLineEdit, row, 3, 1, 1);
    
    m_baseLatTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_baseLatTypeLabel, row, 4, 1, 1);
    row++;

    QString baseLonWhatsThis =
        "The origin for the first longitude line. The first line of the grid"
        " will be drawn at the base longitude. Successive longitude lines will"
        " then be drawn relative to base longitude at an increment defined by"
        " the longitude increment. Base longitude can be outside the range of"
        " the image data.";
    m_baseLonLabel = new QLabel("Base Longitude");
    m_baseLonLabel->setWhatsThis(baseLonWhatsThis);
    mainLayout->addWidget(m_baseLonLabel, row, 0, 1, 2);

    m_baseLonSlider = new QSlider(Qt::Horizontal);
    m_baseLonSlider->setRange(0, 100);
    m_baseLonSlider->setWhatsThis(baseLonWhatsThis);
    connect(m_baseLonSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onBaseLonSliderChanged()));
    mainLayout->addWidget(m_baseLonSlider, row, 2, 1, 1);

    m_baseLonLineEdit = new QLineEdit("0");
    m_baseLonLineEdit->setValidator(new QDoubleValidator(this));
    m_baseLonLineEdit->setWhatsThis(baseLonWhatsThis);
    connect(m_baseLonLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_baseLonLineEdit, row, 3, 1, 1);

    m_baseLonTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_baseLonTypeLabel, row, 4, 1, 1);
    row++;

    QString latIncWhatsThis =
        "The latitude increment is how often a line is drawn as the latitude "
        "values change. A latitude increment of 45 will result in a line at "
        "latitude = -90, -45, 0, 45, 90 for the entire longitude range.";
    m_latIncLabel = new QLabel("Latitude Increment");
    m_latIncLabel->setWhatsThis(latIncWhatsThis);
    mainLayout->addWidget(m_latIncLabel, row, 0, 1, 2);

    m_latIncSlider = new QSlider(Qt::Horizontal);
    m_latIncSlider->setRange(1, 180);
    m_latIncSlider->setWhatsThis(latIncWhatsThis);
    connect(m_latIncSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onLatIncSliderChanged()));
    mainLayout->addWidget(m_latIncSlider, row, 2, 1, 1);
    
    m_latIncLineEdit = new QLineEdit("45");
    m_latIncLineEdit->setValidator(new QDoubleValidator(0, 180.0, 15, this));
    m_latIncLineEdit->setWhatsThis(latIncWhatsThis);
    connect(m_latIncLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_latIncLineEdit, row, 3, 1, 1);

    m_latIncTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_latIncTypeLabel, row, 4, 1, 1);
    row++;

    QString lonIncWhatsThis =
        "The longitude increment is how often a line is drawn as the longitude "
        "values change. A longitude increment of 180 will result in a line at "
        "longitude = 0, 180, 360 for the entire latitude range.";
    m_lonIncLabel = new QLabel("Longitude Increment");
    m_lonIncLabel->setWhatsThis(lonIncWhatsThis);
    mainLayout->addWidget(m_lonIncLabel, row, 0, 1, 2);

    m_lonIncSlider = new QSlider(Qt::Horizontal);
    m_lonIncSlider->setRange(1, 360);
    m_lonIncSlider->setWhatsThis(lonIncWhatsThis);
    connect(m_lonIncSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onLonIncSliderChanged()));
    mainLayout->addWidget(m_lonIncSlider, row, 2, 1, 1);
    
    m_lonIncLineEdit = new QLineEdit("45");
    m_lonIncLineEdit->setValidator(new QDoubleValidator(this));
    m_lonIncLineEdit->setWhatsThis(lonIncWhatsThis);
    connect(m_lonIncLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_lonIncLineEdit, row, 3, 1, 1);

    m_lonIncTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_lonIncTypeLabel, row, 4, 1, 1);
    row++;
    mainLayout->setRowMinimumHeight(row, 10);
    row++;

    QString latExtentWhatsThis =
        "The longitude range determines the extents of the grid. The \"Read Map File\" option will "
        "derive the extents from the loaded map's projection. The \"Compute From Images\" option "
        "will use the ranges covered by the open cubes. The \"Manual\" option allows you to enter "
        "values of your choice.";
    m_latExtentLabel = new QLabel("Latitude Range");
    m_latExtentLabel->setWhatsThis(latExtentWhatsThis);
    mainLayout->addWidget(m_latExtentLabel, row, 0, 1, 2);

    m_latExtentCombo = new QComboBox;
    m_latExtentCombo->addItem("Read Map File", MosaicGridTool::Map);
    m_latExtentCombo->addItem("Compute From Images", MosaicGridTool::Cubes);
    m_latExtentCombo->addItem("Manual", MosaicGridTool::Manual);
    m_latExtentCombo->setCurrentIndex(m_latExtentCombo->findData(m_tool->latExtents()));
    m_latExtentCombo->setWhatsThis(latExtentWhatsThis);
    connect(m_latExtentCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onExtentTypeChanged()));
    mainLayout->addWidget(m_latExtentCombo, row, 2, 1, 2);
    m_latExtentTypeLabel = new QLabel(m_tool->latType());
    mainLayout->addWidget(m_latExtentTypeLabel, row, 4, 1, 1);
    row++;


    QString minLatWhatsThis =
        "The minimum latitude will be the lower edge of the grid. This parameter currently "
        "expects degree input.";
    m_minLatExtentLabel = new QLabel("Minimum Latitude");
    m_minLatExtentLabel->setWhatsThis(minLatWhatsThis);
    mainLayout->addWidget(m_minLatExtentLabel, row, 1, 1, 1);

    m_minLatExtentSlider = new QSlider(Qt::Horizontal);
    m_minLatExtentSlider->setRange(-90, 90);
    m_minLatExtentSlider->setWhatsThis(minLatWhatsThis);
    connect(m_minLatExtentSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onMinLatExtentSliderChanged()));
    mainLayout->addWidget(m_minLatExtentSlider, row, 2, 1, 1);

    m_minLatExtentLineEdit = new QLineEdit("0");
    m_minLatExtentLineEdit->setValidator(new QDoubleValidator(this));
    m_minLatExtentLineEdit->setWhatsThis(minLatWhatsThis);
    connect(m_minLatExtentLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_minLatExtentLineEdit, row, 3, 1, 1);
    m_minLatExtentTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_minLatExtentTypeLabel, row, 4, 1, 1);
    row++;

    QString maxLatWhatsThis =
        "The maximum latitude will be the upper edge of the grid. This parameter currently "
        "expects degree input.";
    m_maxLatExtentLabel = new QLabel("Maximum Latitude");
    m_maxLatExtentLabel->setWhatsThis(maxLatWhatsThis);
    mainLayout->addWidget(m_maxLatExtentLabel, row, 1, 1, 1);

    m_maxLatExtentSlider = new QSlider(Qt::Horizontal);
    m_maxLatExtentSlider->setRange(-90, 90);
    m_maxLatExtentSlider->setWhatsThis(maxLatWhatsThis);
    connect(m_maxLatExtentSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onMaxLatExtentSliderChanged()));
    mainLayout->addWidget(m_maxLatExtentSlider, row, 2, 1, 1);

    m_maxLatExtentLineEdit = new QLineEdit("0");
    m_maxLatExtentLineEdit->setValidator(new QDoubleValidator(this));
    m_maxLatExtentLineEdit->setWhatsThis(maxLatWhatsThis);
    connect(m_maxLatExtentLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_maxLatExtentLineEdit, row, 3, 1, 1);
    m_maxLatExtentTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_maxLatExtentTypeLabel, row, 4, 1, 1);
    row++;
    mainLayout->setRowMinimumHeight(row, 10);
    row++;
    
    QString lonExtentWhatsThis =
        "The longitude range determines the extents of the grid. The \"<b>Read Map File</b>\" "
        "option will derive the extents from the loaded map's projection. The"
        "\"Compute From Images\" option will use the ranges covered by the open cubes. The "
        "\"Manual\" option allows you to enter "
        "values of your choice. The domain is that of the map projection.";
    m_lonExtentLabel = new QLabel("Longitude Range");
    m_lonExtentLabel->setWhatsThis(lonExtentWhatsThis);
    mainLayout->addWidget(m_lonExtentLabel, row, 0, 1, 2);

    m_lonExtentCombo = new QComboBox;
    m_lonExtentCombo->addItem("Read Map File", MosaicGridTool::Map);
    m_lonExtentCombo->addItem("Compute From Images", MosaicGridTool::Cubes);
    m_lonExtentCombo->addItem("Manual", MosaicGridTool::Manual);

    m_lonExtentCombo->setCurrentIndex(m_lonExtentCombo->findData(m_tool->lonExtents()));
    m_lonExtentCombo->setWhatsThis(lonExtentWhatsThis);
    connect(m_lonExtentCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onExtentTypeChanged()));
    mainLayout->addWidget(m_lonExtentCombo, row, 2, 1, 2);

    m_lonDomainLabel = new QLabel(m_tool->lonDomain() + " Domain");
    mainLayout->addWidget(m_lonDomainLabel, row, 4, 1, 1);
    row++;

    QString minLonWhatsThis =
        "The maximum longitude will be the left edge of the grid. This parameter currently "
        "expects degree input.";
    m_minLonExtentLabel = new QLabel("Minimum Longitude");
    m_minLonExtentLabel->setWhatsThis(minLonWhatsThis);
    mainLayout->addWidget(m_minLonExtentLabel, row, 1, 1, 1);

    m_minLonExtentSlider = new QSlider(Qt::Horizontal);
    m_minLonExtentSlider->setRange(0, 360);
    m_minLonExtentSlider->setWhatsThis(minLonWhatsThis);
    connect(m_minLonExtentSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onMinLonExtentSliderChanged()));
    mainLayout->addWidget(m_minLonExtentSlider, row, 2, 1, 1);

    m_minLonExtentLineEdit = new QLineEdit("0");
    m_minLonExtentLineEdit->setValidator(new QDoubleValidator(this));
    m_minLonExtentLineEdit->setWhatsThis(minLonWhatsThis);
    connect(m_minLonExtentLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_minLonExtentLineEdit, row, 3, 1, 1);
    m_minLonExtentTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_minLonExtentTypeLabel, row, 4, 1, 1);
    row++;

    QString maxLonWhatsThis =
        "The maximum longitude will be the right edge of the grid. This parameter currently "
        "expects degree input.";
    m_maxLonExtentLabel = new QLabel("Maximum Longitude");
    m_maxLonExtentLabel->setWhatsThis(maxLonWhatsThis);
    mainLayout->addWidget(m_maxLonExtentLabel, row, 1, 1, 1);

    m_maxLonExtentSlider = new QSlider(Qt::Horizontal);
    m_maxLonExtentSlider->setRange(0, 360);
    m_maxLonExtentSlider->setWhatsThis(maxLonWhatsThis);
    connect(m_maxLonExtentSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onMaxLonExtentSliderChanged()));
    mainLayout->addWidget(m_maxLonExtentSlider, row, 2, 1, 1);

    m_maxLonExtentLineEdit = new QLineEdit("0");
    m_maxLonExtentLineEdit->setValidator(new QDoubleValidator(this));
    m_maxLonExtentLineEdit->setWhatsThis(maxLonWhatsThis);
    connect(m_maxLonExtentLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_maxLonExtentLineEdit, row, 3, 1, 1);
    m_maxLonExtentTypeLabel = new QLabel("Degrees");
    mainLayout->addWidget(m_maxLonExtentTypeLabel, row, 4, 1, 1);
    row++;
    mainLayout->setRowMinimumHeight(row, 10);
    row++;

    QString densityWhatsThis =
        "The density is the estimated total number of straight lines used "
        "to create the grid. Increasing this number will significantly slow "
        "down the drawing of the grid while making curves more accurate. If "
        "the grid does not look accurate then try increasing this number.";
    m_densityLabel = new QLabel("Grid Line Density");
    m_densityLabel->setWhatsThis(densityWhatsThis);
    mainLayout->addWidget(m_densityLabel, row, 0, 1, 2);
    
    m_densityEdit = new QLineEdit("10000");
    m_densityEdit->setValidator(new QIntValidator(1, INT_MAX, this));
    m_densityEdit->setWhatsThis(densityWhatsThis);
    connect(m_densityEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(refreshWidgetStates()));
    mainLayout->addWidget(m_densityEdit, row, 2, 1, 2);
    row++;
    mainLayout->setRowMinimumHeight(row, 10);
    row++;

    QHBoxLayout *buttonsAreaLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsAreaLayout, row, 0, 1, 4);

    QString autoApplyWhatsThis =
        "Automatically updates the grid when parameters are changed.";
    m_autoApplyCheckBox = new QCheckBox("Auto Apply");
    m_autoApplyCheckBox->setChecked(true);
    buttonsAreaLayout->addWidget(m_autoApplyCheckBox);
  
    buttonsAreaLayout->addStretch();

    QPushButton *okayButton = new QPushButton("&Ok");
    okayButton->setIcon(QIcon::fromTheme("dialog-ok"));
    connect(okayButton, SIGNAL(clicked()),
            this, SLOT(applySettings()));
    connect(okayButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    buttonsAreaLayout->addWidget(okayButton);

    QPushButton *applyButton = new QPushButton("&Apply");
    applyButton->setIcon(QIcon::fromTheme("dialog-ok-apply"));
    connect(applyButton, SIGNAL(clicked()),
            this, SLOT(applySettings()));
    buttonsAreaLayout->addWidget(applyButton);

    QPushButton *cancelButton = new QPushButton("&Cancel");
    cancelButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    buttonsAreaLayout->addWidget(cancelButton);

    connect(m_tool, SIGNAL(boundingRectChanged()), this, SLOT(readSettings()));

    readSettings();
  }

  /**
   * Clean up allocated memory.
   */
  MosaicGridToolConfigDialog::~MosaicGridToolConfigDialog() {
  }


  /**
   * Apply the user's current settings to the tool. Draw or clear the grid depending on
   *   the settings.
   */
  void MosaicGridToolConfigDialog::applySettings(bool shouldReadSettings) {

    int cursorPos = 0;
    
    // Validate base latitude value
    QString baseLatitude = m_baseLatLineEdit->text();
    if (m_baseLatLineEdit->isEnabled()) {
      QValidator::State validBaseLat =
          m_baseLatLineEdit->validator()->validate(baseLatitude, cursorPos);
      if (validBaseLat != QValidator::Acceptable) {
        throw IException(IException::Unknown,
              "Base Latitude value must be in the range -90 to 90",
            _FILEINFO_);
      }
    }

    // Validate base longitude value
    QString baseLongitude = m_baseLonLineEdit->text();
    QValidator::State validBaseLon =
        m_baseLonLineEdit->validator()->validate(baseLongitude, cursorPos);
    if (validBaseLon != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Base Longitude value must be a double",
          _FILEINFO_);
    }

    // Validate latitudeInc value
    QString latitudeInc = m_latIncLineEdit->text();
    QValidator::State validLatInc = m_latIncLineEdit->validator()->validate(latitudeInc,
                                                                            cursorPos);
    if (validLatInc != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Latitude increment must be in the range 0 to 180",
          _FILEINFO_);
    }

    // Validate longitudeInc value
    QString longitudeInc = m_lonIncLineEdit->text();
    QValidator::State validLonInc =
        m_lonIncLineEdit->validator()->validate(longitudeInc, cursorPos);
    if (validLonInc != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Longitude increment must be a double",
          _FILEINFO_);
    }
    
    // Validate minLatExtent value
    QString minLatExtent = m_minLatExtentLineEdit->text();
    QValidator::State validMinLatExtent =
        m_minLatExtentLineEdit->validator()->validate(minLatExtent, cursorPos);
    if (validMinLatExtent != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Minimum latitude extent must be a double",
          _FILEINFO_);
    }
    
    // Validate maxLatExtent value
    QString maxLatExtent = m_maxLatExtentLineEdit->text();
    QValidator::State validMaxLatExtent =
        m_maxLatExtentLineEdit->validator()->validate(maxLatExtent, cursorPos);
    if (validMaxLatExtent != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Maximum latitude extent must be a double",
          _FILEINFO_);
    }

    // Validate minLonExtent value
    QString minLonExtent = m_minLonExtentLineEdit->text();
    QValidator::State validMinLonExtent =
        m_minLonExtentLineEdit->validator()->validate(minLonExtent, cursorPos);
    if (validMinLonExtent != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Minimum longitude extent must be a double",
          _FILEINFO_);
    }
    
    // Validate maxLonExtent value
    QString maxLonExtent = m_maxLonExtentLineEdit->text();
    QValidator::State validMaxLonExtent =
        m_maxLonExtentLineEdit->validator()->validate(maxLonExtent, cursorPos);
    if (validMaxLonExtent != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Maximum longitude extent must be a double",
          _FILEINFO_);
    }

    // Validate density value
    QString density = m_densityEdit->text();
    QValidator::State validDensity =
        m_densityEdit->validator()->validate(density, cursorPos);
    if (validDensity != QValidator::Acceptable) {
      throw IException(IException::Unknown,
          "Density must be a non-zero positive integer",
          _FILEINFO_);
    }

    if (m_tool->sceneWidget()->getProjection()) {
      PvlGroup mapGroup(m_tool->sceneWidget()->getProjection()->Mapping());

      m_tool->setShowGrid(m_showGridCheckBox->isChecked());
      m_tool->setAutoGridCheckBox(m_autoGridCheckBox->isChecked());

      m_tool->setBaseLat(Latitude(baseLatitude.toDouble(),  mapGroup, Angle::Degrees));
      m_tool->setBaseLon(Longitude(baseLongitude.toDouble(), Angle::Degrees));
      m_tool->setLatInc(Angle(latitudeInc.toDouble(), Angle::Degrees));
      m_tool->setLonInc(Angle(longitudeInc.toDouble(), Angle::Degrees));

      m_tool->setLatExtents((MosaicGridTool::GridExtentSource)
          m_latExtentCombo->itemData(m_latExtentCombo->currentIndex()).toInt(),
          Latitude(minLatExtent.toDouble(), mapGroup, Angle::Degrees),
          Latitude(maxLatExtent.toDouble(), mapGroup, Angle::Degrees));
      m_tool->setLonExtents((MosaicGridTool::GridExtentSource)
          m_lonExtentCombo->itemData(m_lonExtentCombo->currentIndex()).toInt(),
          Longitude(minLonExtent.toDouble(), Angle::Degrees),
          Longitude(maxLonExtent.toDouble(), Angle::Degrees));

      m_tool->setDensity(density.toInt());

      if (m_showGridCheckBox->isChecked() && m_autoGridCheckBox->isChecked())
        m_tool->autoGrid(m_autoGridCheckBox->isChecked());
      else if (m_showGridCheckBox->isChecked())
        m_tool->drawGrid();
      else
        m_tool->clearGrid();

      if (shouldReadSettings)
        readSettings();
    }
  }


  /**
   * Slot that calls applySettings with true to call readSettings also.
   */
  void MosaicGridToolConfigDialog::applySettings() {
    applySettings(true);
  }


  /**
   * Read the tool's current settings and set the widget states to match.
   */
  void MosaicGridToolConfigDialog::readSettings() {

    if (m_tool->sceneWidget()->getProjection()) {
      // Don't auto apply until we're done
      bool autoApply = m_autoApplyCheckBox->isChecked();
      m_autoApplyCheckBox->setChecked(false);
      
      m_showGridCheckBox->setChecked(m_tool->showGrid());
      m_autoGridCheckBox->setChecked(m_tool->autoGridCheckBox());
      m_baseLonLineEdit->setText(QString::number(m_tool->baseLon().degrees()));

      m_latIncLineEdit->setText(QString::number(m_tool->latInc().degrees()));
      m_lonIncLineEdit->setText(QString::number(m_tool->lonInc().degrees()));

      m_latExtentCombo->setCurrentIndex(m_latExtentCombo->findData(m_tool->latExtents()));
      if (m_tool->sceneWidget()->getProjection()->Mapping()["LatitudeType"][0] ==
          "Planetocentric") {

        m_baseLatLineEdit->setText(QString::number(m_tool->baseLat().degrees()));
        m_minLatExtentLineEdit->setText(QString::number(m_tool->minLat().degrees()));
        m_maxLatExtentLineEdit->setText(QString::number(m_tool->maxLat().degrees()));
      }
      else {

        m_baseLatLineEdit->setText(QString::number(
            m_tool->baseLat().planetographic(Angle::Degrees)));
        m_minLatExtentLineEdit->setText(QString::number(
                                        m_tool->minLat().planetographic(Angle::Degrees)));
        m_maxLatExtentLineEdit->setText(QString::number(
                                        m_tool->maxLat().planetographic(Angle::Degrees)));
      }

      m_lonExtentCombo->setCurrentIndex(m_lonExtentCombo->findData(m_tool->lonExtents()));
      m_minLonExtentLineEdit->setText(QString::number(m_tool->minLon().degrees()));
      m_maxLonExtentLineEdit->setText(QString::number(m_tool->maxLon().degrees()));

      m_densityEdit->setText(QString::number(m_tool->density()));

      // Now we can restore auto apply
      m_autoApplyCheckBox->setChecked(autoApply);
    }
    else {
      refreshWidgetStates(false);
    }
  }

  
  /**
   * Calls the private method refreshWidgetStates with true as the argument
   *   in order to have refreshWidgetStates call apply settings.
   */
  void MosaicGridToolConfigDialog::refreshWidgetStates() {
    refreshWidgetStates(true);
  }


  /**
   * Enables or disables widgets depending on the state of the tool. Also calls apply if
   *   the auto apply checkbox is checked.
   */
  void MosaicGridToolConfigDialog::refreshWidgetStates(bool canAutoApply) {
    
    QElapsedTimer timer;
    timer.start();
    
    bool enabled = m_tool->sceneWidget()->getProjection();
    bool showGrid = enabled && m_showGridCheckBox->isChecked();
    bool autoGrid = enabled && m_autoGridCheckBox->isChecked();
    bool enableLatExtents = ( (MosaicGridTool::GridExtentSource)
        m_latExtentCombo->itemData(m_latExtentCombo->currentIndex()).toInt() ==
        MosaicGridTool::Manual ) && showGrid;
    bool enableLonExtents = ( (MosaicGridTool::GridExtentSource)
        m_lonExtentCombo->itemData(m_lonExtentCombo->currentIndex()).toInt() ==
        MosaicGridTool::Manual ) && showGrid;

    m_autoGridCheckBox->setEnabled(showGrid);

    m_baseLatLabel->setEnabled(showGrid);
    m_baseLatSlider->setEnabled(showGrid);
    m_baseLatSlider->blockSignals(true);
    m_baseLatSlider->setValue(
      qRound(100 * m_baseLatLineEdit->text().toDouble() / m_latIncLineEdit->text().toDouble()));
    m_baseLatSlider->blockSignals(false);
    m_baseLatLineEdit->setEnabled(showGrid);
    m_baseLatTypeLabel->setEnabled(showGrid);

    m_baseLonLabel->setEnabled(showGrid);
    m_baseLonSlider->setEnabled(showGrid);
    m_baseLonSlider->blockSignals(true);
    m_baseLonSlider->setValue(
      qRound(100 * m_baseLonLineEdit->text().toDouble() / m_lonIncLineEdit->text().toDouble()));
    m_baseLonSlider->blockSignals(false);
    m_baseLonLineEdit->setEnabled(showGrid);
    m_baseLonTypeLabel->setEnabled(showGrid);

    m_latIncLabel->setEnabled(showGrid && !autoGrid);
    m_latIncLineEdit->setEnabled(!autoGrid && showGrid);
    m_latIncSlider->blockSignals(true);
    m_latIncSlider->setEnabled(showGrid && !autoGrid);
    m_latIncSlider->setValue(m_latIncLineEdit->text().toDouble());
    m_latIncSlider->blockSignals(false);
    m_latIncTypeLabel->setEnabled(showGrid && !autoGrid);

    m_lonIncLabel->setEnabled(showGrid && !autoGrid);
    m_lonIncLineEdit->setEnabled(showGrid && !autoGrid);
    m_lonIncSlider->blockSignals(true);
    m_lonIncSlider->setEnabled(!autoGrid);
    m_lonIncSlider->setValue(m_lonIncLineEdit->text().toDouble());
    m_lonIncSlider->blockSignals(false);
    m_lonIncTypeLabel->setEnabled(showGrid && !autoGrid);

    m_latExtentLabel->setEnabled(showGrid);
    m_latExtentCombo->setEnabled(showGrid);
    m_latExtentTypeLabel->setEnabled(showGrid);
    
    m_minLatExtentLabel->setEnabled(enableLatExtents);
    m_minLatExtentLabel->setEnabled(enableLatExtents);
    m_minLatExtentSlider->blockSignals(true);
    m_minLatExtentSlider->setValue(
        qRound(m_minLatExtentLineEdit->text().toDouble()));
    m_minLatExtentSlider->blockSignals(false);
    m_minLatExtentLineEdit->setEnabled(enableLatExtents);
    m_minLatExtentTypeLabel->setEnabled(enableLatExtents);
    
    m_maxLatExtentLabel->setEnabled(enableLatExtents);
    m_maxLatExtentSlider->setEnabled(enableLatExtents);
    m_maxLatExtentSlider->blockSignals(true);
    m_maxLatExtentSlider->setValue(
      qRound(m_maxLatExtentLineEdit->text().toDouble()));
    m_maxLatExtentSlider->blockSignals(false);
    m_maxLatExtentLineEdit->setEnabled(enableLatExtents);
    m_maxLatExtentTypeLabel->setEnabled(enableLatExtents);

    m_lonExtentLabel->setEnabled(showGrid);
    m_lonExtentCombo->setEnabled(showGrid);
    m_lonDomainLabel->setEnabled(showGrid);
    
    m_minLonExtentLabel->setEnabled(enableLonExtents);
    m_minLonExtentSlider->setEnabled(enableLonExtents);
    m_minLonExtentSlider->blockSignals(true);
    m_minLonExtentSlider->setValue(m_minLonExtentLineEdit->text().toDouble());
    m_minLonExtentSlider->blockSignals(false);
    m_minLonExtentLineEdit->setEnabled(enableLonExtents);
    m_minLonExtentTypeLabel->setEnabled(enableLonExtents);

    m_maxLonExtentLabel->setEnabled(enableLonExtents);
    m_maxLonExtentSlider->setEnabled(enableLonExtents);
    m_maxLonExtentSlider->blockSignals(true);
    m_maxLonExtentSlider->setValue(m_maxLonExtentLineEdit->text().toDouble());
    m_maxLonExtentSlider->blockSignals(false);
    m_maxLonExtentLineEdit->setEnabled(enableLonExtents);
    m_maxLonExtentTypeLabel->setEnabled(enableLonExtents);

    m_densityLabel->setEnabled(showGrid);
    m_densityEdit->setEnabled(showGrid);
  
    if (m_autoApplyCheckBox->isChecked() && canAutoApply) {
      try {
        if(m_autoGridCheckBox->isChecked()) 
          applySettings(true);
        else
          applySettings(false);
      }
      catch (IException &) {
      }
    
      //Time in milliseconds
      if (timer.elapsed() > 250) {
        m_densityEdit->setText(QString::number(
            qMax(1000, qRound(m_densityEdit->text().toInt() * 0.75))));
      }
    }
  }
    
  /**
   * Updates the corresponding line edit when the baseLatSlider changes.
   */
  void MosaicGridToolConfigDialog::onBaseLatSliderChanged() {
    m_baseLatLineEdit->setText(
      QString::number( (m_baseLatSlider->value() / 100.0) * m_latIncLineEdit->text().toDouble() ));
  }


  /**
   * Updates the corresponding line edit when the baseLonSlider changes.
   */
  void MosaicGridToolConfigDialog::onBaseLonSliderChanged() {
    m_baseLonLineEdit->setText(
      QString::number( (m_baseLonSlider->value() / 100.0) * m_lonIncLineEdit->text().toDouble() ));
  }


  /**
   * Updates the corresponding line edit when the latIncSlider changes.
   */
  void MosaicGridToolConfigDialog::onLatIncSliderChanged() {
    m_latIncLineEdit->setText(QString::number(m_latIncSlider->value()));
  }


  /**
   * Updates the corresponding line edit when the lonIncSlider changes.
   */
  void MosaicGridToolConfigDialog::onLonIncSliderChanged() {
    m_lonIncLineEdit->setText(QString::number(m_lonIncSlider->value()));
  }


  /**
   * Updates the corresponding line edit when the minLatSlider changes.
   */
  void MosaicGridToolConfigDialog::onMinLatExtentSliderChanged() {
    if (m_minLatExtentSlider->value() < m_maxLatExtentSlider->value()) {
      m_minLatExtentLineEdit->setText(QString::number(m_minLatExtentSlider->value()));
    }
    else {
      m_minLatExtentSlider->setValue(m_maxLatExtentSlider->value() - 1);
    }
  }


  /**
   * Updates the corresponding line edit when the maxLatSlider changes.
   */
  void MosaicGridToolConfigDialog::onMaxLatExtentSliderChanged() {
    if (m_maxLatExtentSlider->value() > m_minLatExtentSlider->value()) {
      m_maxLatExtentLineEdit->setText(QString::number(m_maxLatExtentSlider->value()));
    }
    else {
      m_maxLatExtentSlider->setValue(m_minLatExtentSlider->value() + 1);
    }
  }


  /**
   * Updates the dialog when the lat or lon extent source is changed. This is
   *   necessary because the tools values will change and they need to be
   *   brought back to the dialog.
   */
  void MosaicGridToolConfigDialog::onExtentTypeChanged() {
    QElapsedTimer timer;
    timer.start();
    refreshWidgetStates(false);
    applySettings(true);
    //Time in milliseconds
    if (timer.elapsed() > 250)
      m_densityEdit->setText(QString::number(1000));
  }

  
  /**
   * Updates the corresponding line edit when the minLonSlider changes.
   */
  void MosaicGridToolConfigDialog::onMinLonExtentSliderChanged() {
    if (m_minLonExtentSlider->value() < m_maxLonExtentSlider->value()) {
      m_minLonExtentLineEdit->setText(QString::number(m_minLonExtentSlider->value()));
    }
    else {
      m_minLonExtentSlider->setValue(m_maxLonExtentSlider->value() - 1);
    }
  }


  /**
   * Updates the corresponding line edit when the maxLonSlider changes.
   */
  void MosaicGridToolConfigDialog::onMaxLonExtentSliderChanged() {
    if (m_maxLonExtentSlider->value() > m_minLonExtentSlider->value()) {
      m_maxLonExtentLineEdit->setText(QString::number(m_maxLonExtentSlider->value()));
    }
    else {
      m_maxLonExtentSlider->setValue(m_minLonExtentSlider->value() + 1);
    }
  }
}
