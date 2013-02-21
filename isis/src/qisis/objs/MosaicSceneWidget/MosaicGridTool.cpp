#include "MosaicGridTool.h"

#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QDoubleValidator>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPointF>
#include <QPushButton>
#include <QtCore>

#include "Angle.h"
#include "Distance.h"
#include "GridGraphicsItem.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGraphicsView.h"
#include "MosaicGridToolConfigDialog.h"
#include "Projection.h"
#include "MosaicSceneWidget.h"
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

    if (getWidget())
      m_previousBoundingRect = getWidget()->cubesBoundingRect();

    m_shouldCheckBoxes = true;

    m_baseLat = Latitude(0, Angle::Degrees);
    m_baseLon = Longitude(0, Angle::Degrees);

    m_latInc = Angle(45, Angle::Degrees);
    m_lonInc = Angle(45, Angle::Degrees);

    m_latExtents = Cubes;
    m_minLat = Latitude(-50.0, Angle::Degrees);
    m_maxLat = Latitude(50.0, Angle::Degrees);

    m_lonExtents = Cubes;
    m_minLon = Longitude(0.0, Angle::Degrees);
    m_maxLon = Longitude(360.0, Angle::Degrees);
    m_density = 10000;

  }


  /**
   * Adds the pan action to the given menu.
   *
   * @param menu
   */
  void MosaicGridTool::addToMenu(QMenu *menu) {
  }


  /**
   * True if checked
   *
   * @return The state of the checkbox.
   */
  bool MosaicGridTool::autoGridCheckBox() {
    return m_autoGridCheckBox->isChecked();
  }


  /**
   * The base latitude.
   *
   * @return The base latitude
   */
  Latitude MosaicGridTool::baseLat() {
    return m_baseLat;
  }


  /**
   * The base longitude.
   *
   * @return The base longitude
   */
  Longitude MosaicGridTool::baseLon() {
    return m_baseLon;
  }


  /**
   * The density or resolution of the grid. The number of straight lines
   *   used to draw the grid.
   *
   * @return The density
   */
  int MosaicGridTool::density() {
    return m_density;
  }


  /**
   * The angle of the latitude increment.
   *
   * @return The latitude increment angle.
   */
  Angle MosaicGridTool::latInc() {
    return m_latInc;
  }


  /**
   * The extent type (Map, Cubes, Manual) for the latitude.
   *
   * @return The extent type of the latitude
   */
  MosaicGridTool::GridExtentSource MosaicGridTool::latExtents() {
    return m_latExtents;
  }


  /**
   * The latitude type (planetocentric/planetographic) of the projection of the scene.
   *
   * @return the latitude type as a string
   */
  QString MosaicGridTool::latType() {
    QString result;
    if (getWidget()->getProjection())
      result = getWidget()->getProjection()->LatitudeTypeString();
    return result;
  }


  /**
   * The longitude domain of the projection of the scene.
   *
   * @return the domain as a string
   */
  QString MosaicGridTool::lonDomain() {
    QString result;
    if (getWidget()->getProjection())
      result = getWidget()->getProjection()->LongitudeDomainString();
    return result;
  }


  /**
   * The extent type (Map, Cubes, Manual) for the longitude.
   *
   * @return The extent type of the longitude
   */
  MosaicGridTool::GridExtentSource MosaicGridTool::lonExtents() {
    return m_lonExtents;
  }


  /**
   * The angle of the longitude increment.
   *
   * @return The longitude increment angle.
   */
  Angle MosaicGridTool::lonInc() {
    return m_lonInc;
  }


  /**
   * The maximum latitude used to determine the grid's extents and increments
   *
   * @return The maximum latitude of the grid range.
   */
  Latitude MosaicGridTool::maxLat() {
    return m_maxLat;
  }


  /**
   * The maximum longitude used to determine the grid's extents and increments
   *
   * @return The maximum longitude of the grid range.
   */
  Longitude MosaicGridTool::maxLon() {
    return m_maxLon;
  }


  /**
   * The minimum latitude used to determine the grid's extents and increments
   *
   * @return The minimum latitude of the grid range.
   */
  Latitude MosaicGridTool::minLat() {
    return m_minLat;
  }


  /**
   * The minimum longitude used to determine the grid's extents and increments
   *
   * @return  The minimum longitude of the grid range.
   */
  Longitude MosaicGridTool::minLon() {
    return m_minLon;
  }


  /**
   *
   *
   * @return a pointer to the scene
   */
  MosaicSceneWidget* MosaicGridTool::sceneWidget() {
    return getWidget();
  }


  /**
   * True if grid is displayed
   *
   * @return Whether or not the grid is displayed.
   */
  bool MosaicGridTool::showGrid() {
    return m_drawGridCheckBox->isChecked();
  }


  /**
   * Modify the check state of the checkbox.
   *
   * @param checked the new state of the checkbox
   */
  void MosaicGridTool::setAutoGridCheckBox(bool checked) {
    m_autoGridCheckBox->setChecked(checked);
  }


  /**
   * Modify the base latitude.
   *
   * @param baseLat the new base latitude.
   */
  void MosaicGridTool::setBaseLat(Latitude baseLat) {
    m_baseLat = Latitude(baseLat);
  }


  /**
   * Modify the base longitude.
   *
   * @param baseLon the new base longitude.
   */
  void MosaicGridTool::setBaseLon(Longitude baseLon) {
    m_baseLon = baseLon;
  }


  /**
   * Modify the density.
   *
   * @param density the new density value.
   */
  void MosaicGridTool::setDensity(int density) {
    m_density = density;
  }


  /**
   * Set the maximum and minimum latitude of the grid.
   *
   * @param source Where the grid extents come from (Map, Cubes, Manual).
   * @param minLat The minimum latitude of the grid.
   * @param maxLat The maximum latitude of the grid.
   */
  void MosaicGridTool::setLatExtents(GridExtentSource source,
                                     Latitude minLat = Latitude(),
                                     Latitude maxLat = Latitude()) {
    m_latExtents = source;

    Projection *proj = getWidget()->getProjection();
    if (proj) {
      PvlGroup mappingGroup(proj->Mapping());

      Distance equatorialRadius(toDouble(proj->Mapping()["EquatorialRadius"][0]),
                                Distance::Meters);
      Distance polarRadius(toDouble(proj->Mapping()["PolarRadius"][0]), Distance::Meters);

      QRectF boundingRect = getWidget()->cubesBoundingRect();

      double topLeft = 100;
      double topRight = 100;
      double bottomLeft = 100;
      double bottomRight = 100;
      bool cubeRectWorked = true;

      switch (source) {

        case Map:
          m_minLat = Latitude(proj->MinimumLatitude(), mappingGroup, Angle::Degrees);
          m_maxLat = Latitude(proj->MaximumLatitude(), mappingGroup,  Angle::Degrees);
          break;

        case Cubes:
          if (proj->SetCoordinate(boundingRect.topLeft().x(), -boundingRect.topLeft().y())) {
            topLeft = proj->Latitude();
          }
          else {
            cubeRectWorked = false;
          }
          if (proj->SetCoordinate(boundingRect.topRight().x(), -boundingRect.topRight().y())) {
            topRight = proj->Latitude();
          }
          else {
            cubeRectWorked = false;
          }
          if (proj->SetCoordinate(boundingRect.bottomLeft().x(), -boundingRect.bottomLeft().y())) {
            bottomLeft = proj->Latitude();
          }
          else {
            cubeRectWorked = false;
          }
          if (proj->SetCoordinate(boundingRect.bottomRight().x(),
              -boundingRect.bottomRight().y())) {
            bottomRight = proj->Latitude();
          }
          else {
            cubeRectWorked = false;
          }

          if (cubeRectWorked) {
            m_minLat = Latitude(std::min(std::min(topLeft, topRight),
                                        std::min(bottomLeft, bottomRight)), mappingGroup,
                                Angle::Degrees);
            m_maxLat = Latitude(std::max(std::max(topLeft, topRight),
                                        std::max(bottomLeft, bottomRight)), mappingGroup,
                                Angle::Degrees);

            if (proj->SetUniversalGround(-90.0, 0) &&
                boundingRect.contains(QPointF(proj->XCoord(), -proj->YCoord()))) {
              m_minLat = Latitude(-90.0, mappingGroup, Angle::Degrees);
            }

            if (proj->SetUniversalGround(90.0, 0) &&
                boundingRect.contains(QPointF(proj->XCoord(), -proj->YCoord()))) {
              m_maxLat = Latitude(90.0, mappingGroup, Angle::Degrees);
            }
          }
          else {
            m_minLat = Latitude(proj->MinimumLatitude(), mappingGroup, Angle::Degrees);
            m_maxLat = Latitude(proj->MaximumLatitude(), mappingGroup, Angle::Degrees);
            throw IException(IException::Unknown,
                  "Could not extract latitude extents from the cubes. The extents from the"
                  " projection were used instead.",
                _FILEINFO_);
          }
          break;

        case Manual:
          m_minLat = Latitude(minLat);
          m_maxLat = Latitude(maxLat);
          break;

        default:
          m_minLat = Latitude(proj->MinimumLatitude(), mappingGroup, Angle::Degrees);
          m_maxLat = Latitude(proj->MaximumLatitude(), mappingGroup, Angle::Degrees);
      }
    }
  }


  /**
   * Modify the latitude increment.
   *
   * @param latInc the new increment angle.
   */
  void MosaicGridTool::setLatInc(Angle latInc) {
    if (latInc > Angle(0.0, Angle::Degrees)) {
      m_latInc = latInc;
    }
  }


  /**
   * Set the maximum and minimum longitude of the grid.
   *
   * @param source Where the grid extents come from (Map, Cubes, Manual).
   * @param minLon The minimum longitude of the grid.
   * @param maxLon The maximum longitude of the grid.
   */
  void MosaicGridTool::setLonExtents(GridExtentSource source,
                                     Longitude minLon = Longitude(),
                                     Longitude maxLon = Longitude()) {
    m_lonExtents = source;

    Projection *proj = getWidget()->getProjection();
    if (proj) {
      QRectF boundingRect = getWidget()->cubesBoundingRect();

      double topLeft = 0;
      double topRight = 0;
      double bottomLeft = 0;
      double bottomRight = 0;
      bool cubeRectWorked = true;

      Longitude zeroLon(0.0, Angle::Degrees);
      Longitude threeSixtyLon(360.0, Angle::Degrees);

      switch (source) {

        case Map:
          m_minLon = Longitude(proj->MinimumLongitude(), Angle::Degrees);
          m_maxLon = Longitude(proj->MaximumLongitude(), Angle::Degrees);
          break;

        case Cubes:
          if (proj->SetCoordinate(boundingRect.topLeft().x(), -boundingRect.topLeft().y())) {
            topLeft = proj->Longitude();
          }
          else {
            cubeRectWorked = false;
          }
          if (proj->SetCoordinate(boundingRect.topRight().x(), -boundingRect.topRight().y())) {
            topRight = proj->Longitude();
          }
          else {
            cubeRectWorked = false;
          }
          if (proj->SetCoordinate(boundingRect.bottomLeft().x(), -boundingRect.bottomLeft().y())) {
            bottomLeft = proj->Longitude();
          }
          else {
            cubeRectWorked = false;
          }
          if (proj->SetCoordinate(boundingRect.bottomRight().x(),
              -boundingRect.bottomRight().y())) {
            bottomRight = proj->Longitude();
          }
          else {
            cubeRectWorked = false;
          }

          if (cubeRectWorked) {
            m_minLon = Longitude(std::min(std::min(topLeft, topRight),
                                          std::min(bottomLeft, bottomRight)),
                                Angle::Degrees);
            m_maxLon = Longitude(std::max(std::max(topLeft, topRight),
                                          std::max(bottomLeft, bottomRight)),
                                Angle::Degrees);
            if (m_minLon < zeroLon) {
              m_minLon = zeroLon;
            }
            if (m_maxLon > threeSixtyLon) {
              m_maxLon = threeSixtyLon;
            }
            //Draw 0-360 if the pole is in the cubes' bounding rectangle.
            if (m_minLat == Angle(-90.0, Angle::Degrees) ||
                m_maxLat == Angle(90.0, Angle::Degrees)) {
              m_minLon = zeroLon;
              m_maxLon = threeSixtyLon;
            }
          }
          else {
            m_minLat = Latitude(proj->MinimumLongitude(), Angle::Degrees);
            m_maxLat = Latitude(proj->MaximumLongitude(), Angle::Degrees);
            throw IException(IException::Unknown,
                  "Could not extract latitude extents from the cubes. The extents from the"
                  " projection were used instead.",
                _FILEINFO_);
          }
          break;

        case Manual:
          m_minLon = minLon;
          m_maxLon = maxLon;
          break;

        default:
          m_minLon = Longitude(proj->MinimumLongitude(), Angle::Degrees);
          m_maxLon = Longitude(proj->MaximumLongitude(), Angle::Degrees);
      }
    }
  }


  /**
   * Modify the longitude increment.
   *
   * @param lonInc the new lonitude increment.
   */
  void MosaicGridTool::setLonInc(Angle lonInc) {
    Angle lonRange = m_maxLon - m_minLon;

    if (lonInc > lonRange)
      m_lonInc = lonRange;
    else if (lonInc > Angle(0.0, Angle::Degrees))
      m_lonInc = lonInc;
  }


  /**
   * Modify the check state of the checkbox.
   *
   * @param checked the new state of the checkbox
   */
  void MosaicGridTool::setShowGrid(bool show) {
    m_drawGridCheckBox->setChecked(show);
  }


  /**
   * Read the tool information form a pvl object.
   *
   * @param obj the object from which we are extracting the information
   */
  void MosaicGridTool::fromPvl(const PvlObject &obj) {

    if (getWidget()->getProjection()) {
      Distance equatorialRadius(
          toDouble(getWidget()->getProjection()->Mapping()["EquatorialRadius"][0]),
                                Distance::Meters);
      Distance polarRadius(
          toDouble(getWidget()->getProjection()->Mapping()["PolarRadius"][0]),
                                Distance::Meters);

      if (obj["BaseLatitude"][0] != "Null")
        m_baseLat = Latitude(toDouble(obj["BaseLatitude"][0]), equatorialRadius, polarRadius,
                            Latitude::Planetocentric, Angle::Degrees);

      if (obj["BaseLongitude"][0] != "Null")
        m_baseLon = Longitude(toDouble(obj["BaseLongitude"][0]), Angle::Degrees);

      if (obj["LatitudeIncrement"][0] != "Null")
        m_latInc = Angle(toDouble(obj["LatitudeIncrement"][0]), Angle::Degrees);

      if (obj["LongitudeIncrement"][0] != "Null")
        m_lonInc = Angle(toDouble(obj["LongitudeIncrement"][0]), Angle::Degrees);

      if (obj.HasKeyword("LatitudeExtentType")) {
        if (obj["LatitudeExtentType"][0] != "Null")
          m_latExtents = (GridExtentSource)toInt(obj["LatitudeExtentType"][0]);
      }

      if (obj.HasKeyword("MinimumLatitude")) {
        if (obj["MinimumLatitude"][0] != "Null")
          m_minLat = Latitude(toDouble(obj["MinimumLatitude"][0]), equatorialRadius, polarRadius,
                              Latitude::Planetocentric, Angle::Degrees);
      }

      if (obj.HasKeyword("MaximumLatitude")) {
        if (obj["MaximumLatitude"][0] != "Null")
          m_maxLat = Latitude(toDouble(obj["MaximumLatitude"][0]), equatorialRadius, polarRadius,
                              Latitude::Planetocentric, Angle::Degrees);
      }

      if (obj.HasKeyword("LongitudeExtentType")) {
        if (obj["LongitudeExtentType"][0] != "Null")
          m_lonExtents = (GridExtentSource)toInt(obj["LongitudeExtentType"][0]);
      }

      if (obj.HasKeyword("MinimumLongitude")) {
        if (obj["MinimumLongitude"][0] != "Null")
          m_minLon = Longitude(toDouble(obj["MinimumLongitude"][0]), Angle::Degrees);
      }

      if (obj.HasKeyword("MaximumLongitude")) {
        if (obj["MaximumLongitude"][0] != "Null")
          m_maxLon = Longitude(toDouble(obj["MaximumLongitude"][0]), Angle::Degrees);
      }

      if (obj["Density"][0] != "Null")
        m_density = toDouble(obj["Density"][0]);


      if (obj.HasKeyword("CheckTheBoxes")) {
        if (obj["CheckTheBoxes"][0] != "Null") {
          m_shouldCheckBoxes = (obj["CheckTheBoxes"][0] == "true");
        }
      }

      if(toBool(obj["Visible"][0])) {
        drawGrid();
      }
    }
  }


  /**
   * An accessor for the name of the Pvl object that the tool's information is stored in.
   *
   * @return The name in string form.
   */
  QString MosaicGridTool::projectPvlObjectName() const {
    return "MosaicGridTool";
  }


  /**
   * Store the tool information in a pvl object.
   *
   * @return the pvl object
   */
  PvlObject MosaicGridTool::toPvl() const {
    PvlObject obj(projectPvlObjectName());

    obj += PvlKeyword("ShouldCheckBoxes", toString((int)m_shouldCheckBoxes));

    obj += PvlKeyword("BaseLatitude", toString(m_baseLat.degrees()));
    obj += PvlKeyword("BaseLongitude", toString(m_baseLon.degrees()));

    obj += PvlKeyword("LatitudeIncrement", toString(m_latInc.degrees()));
    obj += PvlKeyword("LongitudeIncrement", toString(m_lonInc.degrees()));

    obj += PvlKeyword("LatitudeExtentType", toString(m_latExtents));
    obj += PvlKeyword("MaximumLatitude", toString(m_maxLat.degrees()));
    obj += PvlKeyword("MinimumLongitude", toString(m_minLon.degrees()));

    obj += PvlKeyword("LongitudeExtentType", toString(m_lonExtents));
    obj += PvlKeyword("MinimumLatitude", toString(m_minLat.degrees()));
    obj += PvlKeyword("MaximumLongitude", toString(m_maxLon.degrees()));

    obj += PvlKeyword("Density", toString(m_density));
    obj += PvlKeyword("Visible", toString((int)(m_gridItem != NULL)));

    return obj;
  }


  /**
   * Calculates the lat/lon increments from the bounding rectangle of the open cubes.
   *
   * @param draw True if lat/lon increments need to be calculated.
   */
  void MosaicGridTool::autoGrid(bool draw) {
    if (draw && getWidget()->getProjection()) {

      QRectF boundingRect = getWidget()->cubesBoundingRect();

      if (!boundingRect.isNull()) {

        setLatExtents(m_latExtents, m_minLat, m_maxLat);
        setLonExtents(m_lonExtents, m_minLon, m_maxLon);

        double latRange = m_maxLat.degrees() - m_minLat.degrees();

        if (getWidget()->getProjection()->Mapping()["LatitudeType"][0] == "Planetographic") {
          latRange =
              m_maxLat.planetographic(Angle::Degrees) - m_minLat.planetographic(Angle::Degrees);
        }

        double lonRange = m_maxLon.degrees() - m_minLon.degrees();

        /*
         * To calculate the lat/lon increments we divide the range by 10 (so we end up
         *   with about 10 sections in the range, whatever the extents may be) and we
         *   divide that by 1, 10, 100,... depending on the log10 of the range. We then
         *   round this value and multiply but the same number that we divided  by. This
         *   gives us a clear, sensible value for an increment.
         *
         *   Example Increments:
         *     Range = 1    --> Inc = .1
         *     Range = 10   --> Inc = 1
         *     Range = 100  --> Inc = 10
         *     Range = 5000 --> Inc = 500
         *
         *   inc = round[(range/10) / 10^floor(log(range) - 1)] * 10^floor(log(range) - 1)
         */

        double latOffsetMultiplier = pow(10, qFloor(log10(latRange) - 1));
        double lonOffsetMultiplier = pow(10, qFloor(log10(lonRange) - 1));

        double idealLatInc = latRange / 10.0;
        double idealLonInc = lonRange / 10.0;

        double roundedLatInc = qRound(idealLatInc / latOffsetMultiplier) * latOffsetMultiplier ;
        double roundedLonInc = qRound(idealLonInc / lonOffsetMultiplier) * lonOffsetMultiplier ;

        m_latInc = Angle(roundedLatInc, Angle::Degrees);
        m_lonInc = Angle(roundedLonInc, Angle::Degrees);

        m_previousBoundingRect = boundingRect;

        drawGrid();
      }
    }
  }


  /**
   * Clears the grid from the scene. Does not erase any grid information.
   */
  void MosaicGridTool::clearGrid() {
    if(m_gridItem != NULL) {
      disconnect(getWidget(), SIGNAL(projectionChanged(Projection *)),
                 this, SLOT(drawGrid()));

      getWidget()->getScene()->removeItem(m_gridItem);

      delete m_gridItem;
      m_gridItem = NULL;
    }
  }


  /**
   * Give a configuration dialog for the options available in this tool.
   */
  void MosaicGridTool::configure() {
    MosaicGridToolConfigDialog *configDialog =
        new MosaicGridToolConfigDialog(this,
                                         qobject_cast<QWidget *>(parent()));
    configDialog->setAttribute(Qt::WA_DeleteOnClose);
    configDialog->show();
  }


  /**
   * Creates the GridGraphicsItem that will draw the grid. If there is no grid item the grid
   *   is cleared and redrawn with a new item.
   *
   */
  void MosaicGridTool::drawGrid() {
    if(m_gridItem != NULL) {
      m_drawGridCheckBox->setChecked(false);
      m_autoGridCheckBox->setEnabled(true);
      m_autoGridLabel->setEnabled(true);
    }

    m_drawGridCheckBox->blockSignals(true);
    m_drawGridCheckBox->setChecked(true);
    m_drawGridCheckBox->blockSignals(false);

    if (!getWidget()->getProjection()) {
      IString msg = "Please set the mosaic scene's projection before trying to "
                    "draw a grid. This means either open a cube (a projection "
                    "will be calculated) or set the projection explicitly";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (m_minLon.degrees() < m_maxLon.degrees() && m_minLat.degrees() < m_maxLat.degrees()) {
      m_gridItem = new GridGraphicsItem(m_baseLat, m_baseLon, m_latInc, m_lonInc, getWidget(),
                                        m_density, m_minLat, m_maxLat, m_minLon, m_maxLon);
    }

    connect(getWidget(), SIGNAL(projectionChanged(Projection *)),
            this, SLOT(drawGrid()), Qt::UniqueConnection);

    connect(getWidget(), SIGNAL(cubesChanged()),
            this, SLOT(onCubesChanged()));

    if (m_gridItem != NULL)
      getWidget()->getScene()->addItem(m_gridItem);
  }


  /**
   * Determines whether the grid should be drawn or not.
   *
   * @param draw True if grid should be drawn. Otherwise, it will be cleared.
   */
  void MosaicGridTool::drawGrid(bool draw) {
    if (draw) {
      m_autoGridLabel->setEnabled(true);
      m_autoGridCheckBox->setEnabled(true);
      drawGrid();
    }
    else {
      clearGrid();
      m_autoGridLabel->setEnabled(false);
      m_autoGridCheckBox->setEnabled(false);
    }
  }


  /**
   * Determines whether or not the bounding rectangle was changed by the addition
   *   or removal of cubes. If it wasn't changed, the grid is not redrawn. If it
   *   was (and autogrid is checked), the grid is redrawn with new lat/lon
   *   increments.
   *
   */
  void MosaicGridTool::onCubesChanged() {
    if (m_previousBoundingRect != getWidget()->cubesBoundingRect()) {
      emit boundingRectChanged();
      autoGrid(m_autoGridCheckBox->isChecked());

      //Make sure that the grid is updated the first time new cubes are opened.
      getWidget()->getView()->update();
      QApplication::processEvents();
    }
  }


  /**
   * Checks both checkboxes when the tool is first opened. Allows the grid to remain
   *   when the tool is not active.
   *
   * @param check True when the tool is activated
   */
  void MosaicGridTool::onToolOpen(bool check) {
    if (check && m_shouldCheckBoxes) {
      m_autoGridCheckBox->setChecked(true);
      m_autoGridCheckBox->setEnabled(true);
      m_autoGridLabel->setEnabled(true);
      m_drawGridCheckBox->setChecked(true);
      m_shouldCheckBoxes = false;
    }
  }


  /**
   * Creates the widget to add to the tool bar.
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
   * Adds the action to the toolpad.
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


  /**
   * Creates the Grid Toolbar Widget
   *
   * @return The toolbar widget
   */
  QWidget *MosaicGridTool::getToolBarWidget() {

    m_previousBoundingRect =  getWidget()->cubesBoundingRect();

    QHBoxLayout *actionLayout = new QHBoxLayout();

    QString autoGridWhatsThis =
        "Draws a grid based on the current lat/lon extents (from the cubes, map, or user).";
    m_autoGridLabel = new QLabel("Auto Grid");
    m_autoGridLabel->setWhatsThis(autoGridWhatsThis);
    m_autoGridCheckBox = new QCheckBox;
    m_autoGridCheckBox->setWhatsThis(autoGridWhatsThis);
    connect(m_autoGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(autoGrid(bool)));
    actionLayout->addWidget(m_autoGridLabel);
    actionLayout->addWidget(m_autoGridCheckBox);

    // Create the action buttons
    QPushButton *optionsButton = new QPushButton("Grid Options");
    optionsButton->setWhatsThis("Opens a dialog box that has the options to change the base"
                                " latitude, base longitude, latitude increment, longitude"
                                " increment, and grid density.");
    connect(optionsButton, SIGNAL(clicked()), this, SLOT(configure()));
    actionLayout->addWidget(optionsButton);

    QString drawGridWhatsThis =
        "Draws a grid based on the current lat/lon extents (from the cubes, map, or user).";
    QLabel *drawGridLabel = new QLabel("Show Grid");
    drawGridLabel->setWhatsThis(drawGridWhatsThis);
    m_drawGridCheckBox = new QCheckBox;
    m_drawGridCheckBox->setWhatsThis(drawGridWhatsThis);
    connect(m_drawGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(drawGrid(bool)));
    actionLayout->addWidget(drawGridLabel);
    actionLayout->addWidget(m_drawGridCheckBox);

    connect(this, SIGNAL(activated(bool)), this, SLOT(onToolOpen(bool)));

    actionLayout->addStretch(1);
    actionLayout->setMargin(0);

    QWidget *toolBarWidget = new QWidget;
    toolBarWidget->setLayout(actionLayout);

    return toolBarWidget;
  }
}
