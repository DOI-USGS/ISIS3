#include "MosaicGridTool.h"

#include <QDialog>
#include <QDoubleValidator>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>

#include "Angle.h"
#include "GridGraphicsItem.h"
#include "IException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"
#include "PvlObject.h"

namespace Isis {
  /**
   * MosaicGridTool constructor
   *
   *
   * @param parent
   */
  MosaicGridTool::MosaicGridTool(MosaicSceneWidget *scene) :
      MosaicTool(scene) {
    m_gridItem = NULL;
  }


  /**
   *
   *
   */
  void MosaicGridTool::getUserGrid() {
    if(m_gridItem != NULL) {
      clearGrid();
    }

    if (!getWidget()->getProjection()) {
      iString msg = "Please set the mosaic scene's projection before trying to "
                    "draw a grid. This means either open a cube (a projection "
                    "will be calculated) or set the projection explicitly";
      throw IException(IException::User, msg, _FILEINFO_);
    }


    // Validate base latitude value
    QString baseLatitude = m_baseLatLineEdit->text();
    int cursorPos = 0;
    QValidator::State validBaseLat =
      m_baseLatLineEdit->validator()->validate(baseLatitude, cursorPos);
    if(validBaseLat != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
          "Base Latitude value must be in the range -90 to 90",
          QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }


    // Validate base longitude value
    QString baseLongitude = m_baseLonLineEdit->text();
    QValidator::State validBaseLon =
      m_baseLonLineEdit->validator()->validate(baseLongitude, cursorPos);
    if(validBaseLon != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
          "Base Longitude value must be a double",
          QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }

    // Validate base latitudeInc value
    QString latitudeInc = m_latIncLineEdit->text();
    QValidator::State validLatInc =
      m_latIncLineEdit->validator()->validate(latitudeInc, cursorPos);
    if(validLatInc != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
          "Latitude increment must be in the range 0 to 180",
          QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }


    // Validate base longitudeInc value
    QString longitudeInc = m_lonIncLineEdit->text();
    QValidator::State validLonInc =
      m_lonIncLineEdit->validator()->validate(longitudeInc, cursorPos);
    if(validLonInc != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
          "Longitude increment must be a double",
          QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }


    // Validate base longitudeInc value
    QString density = m_densityEdit->text();
    QValidator::State validDensity =
      m_densityEdit->validator()->validate(density, cursorPos);
    if(validDensity != QValidator::Acceptable) {
      QMessageBox::warning(getWidget(), "Error",
          "Density must be a non-zero positive integer",
          QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }

    m_gridItem = new GridGraphicsItem(
        Latitude(baseLatitude.toDouble(), Angle::Degrees),
        Longitude(baseLongitude.toDouble(), Angle::Degrees),
        Angle(latitudeInc.toDouble(), Angle::Degrees),
        Angle(longitudeInc.toDouble(), Angle::Degrees),
        getWidget(), density.toInt());

    connect(getWidget(), SIGNAL(projectionChanged(Projection *)),
            this, SLOT(getUserGrid()), Qt::UniqueConnection);

    getWidget()->getScene()->addItem(m_gridItem);
  }


  /**
   * Adds the action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MosaicGridTool::getPrimaryAction() {
    m_action = new QAction(this);
    m_action->setIcon(getIcon("grid.png"));
    m_action->setToolTip("Grid (g)");
    m_action->setShortcut(Qt::Key_G);
    QString text  =
      "<b>Function:</b>  Superimpose a map grid over the area of displayed "
      "footprints in the 'mosaic scene.'<br><br>"
      "This tool allows you to overlay a ground grid onto the mosaic scene. "
      "The inputs are standard ground grid parameters and a grid density."
      "<p><b>Shortcut:</b>  g</p> ";
    m_action->setWhatsThis(text);
    return m_action;
  }


  QWidget *MosaicGridTool::getToolBarWidget() {
    // What's This' provided here were written by Tammy Becker (2011-12-05) with
    //   help from Steven Lambright.
    m_baseLatLineEdit = new QLineEdit;
    m_baseLatLineEdit->setValidator(new QDoubleValidator(-90.0, 90.0, 99, this));
    m_baseLatLineEdit->setWhatsThis(
        "The origin for the first latitude line. The first line of the grid "
        "will be drawn at the base latitude. Successive latitude lines will "
        "then be drawn relative to base latitude at an increment defined by "
        "the latitude increment. Base latitude can be outside the range of "
        "the image data.");
    m_baseLatLineEdit->setText("0");

    m_baseLonLineEdit = new QLineEdit;
    m_baseLonLineEdit->setValidator(new QDoubleValidator(this));
    m_baseLonLineEdit->setWhatsThis(
        "The origin for the first longitude line. The first line of the grid "
        "will be drawn at the base longitude. Successive longitude lines will "
        "then be drawn relative to base longitude at an increment defined by "
        "the longitude increment. Base longitude can be outside the range of "
        "the image data.");
    m_baseLonLineEdit->setText("0");

    m_latIncLineEdit = new QLineEdit;
    m_latIncLineEdit->setValidator(new QDoubleValidator(0, 180.0, 15, this));
    m_latIncLineEdit->setWhatsThis(
        "The latitude increment is how often a line is drawn as the latitude "
        "values change. A latitude increment of 45 will result in a line at "
        "latitude = -90, -45, 0, 45, 90 for the entire longitude range.");
    m_latIncLineEdit->setText("45");

    m_lonIncLineEdit = new QLineEdit;
    m_lonIncLineEdit->setValidator(new QDoubleValidator(this));
    m_lonIncLineEdit->setWhatsThis(
        "The longitude increment is how often a line is drawn as the longitude "
        "values change. A longitude increment of 180 will result in a line at "
        "longitude = 0, 180, 360 for the entire latitude range.");
    m_lonIncLineEdit->setText("45");

    m_densityEdit = new QLineEdit;
    m_densityEdit->setValidator(new QIntValidator(1, INT_MAX, this));
    m_densityEdit->setWhatsThis(
        "The density is the estimated total number of straight lines used "
        "to create the grid. Increasing this number will significantly slow "
        "down the drawing of the grid while making curves more accurate. If "
        "the grid does not look accurate then try increasing this number.");
    m_densityEdit->setText("10000");

    QLabel *baseLatLabel = new QLabel("Base Latitude");
    baseLatLabel->setWhatsThis(m_baseLatLineEdit->whatsThis());
    QLabel *baseLonLabel = new QLabel("Base Longitude");
    baseLonLabel->setWhatsThis(m_baseLonLineEdit->whatsThis());

    QLabel *latIncLabel = new QLabel("Latitude Increment");
    latIncLabel->setWhatsThis(m_latIncLineEdit->whatsThis());
    QLabel *lonIncLabel = new QLabel("Longitude Increment");
    lonIncLabel->setWhatsThis(m_lonIncLineEdit->whatsThis());

    QLabel *densityLabel = new QLabel("Grid Line Density");
    densityLabel->setWhatsThis(m_densityEdit->whatsThis());

    // Create the action buttons
    QPushButton *okButton = new QPushButton("Draw Grid");
    okButton->setWhatsThis("Immediately draw grid lines overlayed on top of "
        "the displayed footprints and optional image data in the "
        "'mosaic scene'. If the optional parameters are modified then this "
        "will update and redraw the newly defined map grid. Footprints or "
        "optional image data must be displayed in the 'mosaic scene' first in "
        "order to draw the map grid");
    connect(okButton, SIGNAL(clicked()), this, SLOT(getUserGrid()));

    QPushButton *clearButton = new QPushButton("Clear Grid");
    clearButton->setWhatsThis("Delete the  displayed map grid from the "
        "'mosaic scene.'");
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearGrid()));

    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(baseLatLabel);
    actionLayout->addWidget(m_baseLatLineEdit);
    actionLayout->addWidget(baseLonLabel);
    actionLayout->addWidget(m_baseLonLineEdit);
    actionLayout->addWidget(latIncLabel);
    actionLayout->addWidget(m_latIncLineEdit);
    actionLayout->addWidget(lonIncLabel);
    actionLayout->addWidget(m_lonIncLineEdit);
    actionLayout->addWidget(densityLabel);
    actionLayout->addWidget(m_densityEdit);
    actionLayout->addWidget(okButton);
    actionLayout->addWidget(clearButton);
    actionLayout->addStretch(1);
    actionLayout->setMargin(0);

    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->setLayout(actionLayout);

    return toolBarWidget;
  }


  /**
   * Adds the pan action to the given menu.
   *
   *
   * @param menu
   */
  void MosaicGridTool::addToMenu(QMenu *menu) {

  }


  PvlObject MosaicGridTool::toPvl() const {
    PvlObject obj(projectPvlObjectName());

    obj += PvlKeyword("BaseLatitude", m_baseLatLineEdit->text());
    obj += PvlKeyword("BaseLongitude", m_baseLonLineEdit->text());
    obj += PvlKeyword("LatitudeIncrement", m_latIncLineEdit->text());
    obj += PvlKeyword("LongitudeIncrement", m_lonIncLineEdit->text());
    obj += PvlKeyword("Density", m_densityEdit->text());
    obj += PvlKeyword("Visible", (m_gridItem != NULL));

    return obj;
  }


  void MosaicGridTool::fromPvl(const PvlObject &obj) {
    if (obj["BaseLatitude"][0] != "Null")
      m_baseLatLineEdit->setText(obj["BaseLatitude"][0]);

    if (obj["BaseLongitude"][0] != "Null")
      m_baseLonLineEdit->setText(obj["BaseLongitude"][0]);

    if (obj["LatitudeIncrement"][0] != "Null")
      m_latIncLineEdit->setText(obj["LatitudeIncrement"][0]);

    if (obj["LongitudeIncrement"][0] != "Null")
      m_lonIncLineEdit->setText(obj["LongitudeIncrement"][0]);

    if (obj["Density"][0] != "Null")
      m_densityEdit->setText(obj["Density"][0]);

    if((int)obj["Visible"][0] != 0) {
      getUserGrid();
    }
  }


  iString MosaicGridTool::projectPvlObjectName() const {
    return "MosaicGridTool";
  }


  /**
   * Creates the widget to add to the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *MosaicGridTool::createToolBarWidget() {
    QWidget *widget = new QWidget();
    return widget;
  }


  /**
   *
   *
   */
  void MosaicGridTool::clearGrid() {
    if(m_gridItem != NULL) {
      disconnect(getWidget(), SIGNAL(projectionChanged(Projection *)),
                 this, SLOT(getUserGrid()));

      getWidget()->getScene()->removeItem(m_gridItem);

      delete m_gridItem;
      m_gridItem = NULL;
    }
  }
}

