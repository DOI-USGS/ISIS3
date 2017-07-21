#include "EphemeridesPlotTool.h"

#include <geos/geom/Point.h>

#include <QDebug>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "Brick.h"
#include "Camera.h"
#include "CubePlotCurve.h"
#include "Histogram.h"
#include "EphemeridesPlotWindow.h"
#include "MdiCubeViewport.h"
#include "PolygonTools.h"
#include "RubberBandComboBox.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"
#include "ToolPad.h"


namespace Isis {

  /**
   * Constructor creates a new EphemeridesPlotTool object.
   *
   * @param parent The parent widget for the tool
   */
  EphemeridesPlotTool::EphemeridesPlotTool(QWidget *parent) : AbstractPlotTool(parent) {
    m_action = new QAction(this);
    m_action->setText("Ephemerides Plot Tool");
    m_action->setIcon(QPixmap(toolIconDir() + "/histogram.png"));
  }


  /**
   * This method is called when the tool is activated by the
   * parent. It enables the rubber band tool which is used to select the cube
   * to view data from.
   */
  void EphemeridesPlotTool::enableRubberBandTool() {
    rubberBandTool()->setDrawActiveViewportOnly(true);
  }


  /**
   * This method adds the ephemerides tool to the tool pad.
   *
   * @param toolpad The tool pad to add this tool to
   *
   * @return QAction* The action that activates the tool
   */
  QAction *EphemeridesPlotTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir() + "/histogram.png"));
    action->setToolTip("Ephemerides");
    action->setShortcut(Qt::Key_H);

    QString text  =
      "<b>Function:</b>  Plot ephemerides in active viewport";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Forget the position and rotation curves.
   */
  void EphemeridesPlotTool::detachCurves() {
    m_xCurve = NULL;
    m_yCurve = NULL;
    m_zCurve = NULL;
    m_raCurve = NULL;
    m_decCurve = NULL;
    m_twiCurve = NULL;
  }


  /**
   * This method creates the default plot window.
   * 
   * @return @b PlotWindow* A pointer to the created window.
   */
  PlotWindow *EphemeridesPlotTool::createWindow() {
    PlotWindow *window = new EphemeridesPlotWindow(
        "Ephemerides " + PlotWindow::defaultWindowTitle(),
        qobject_cast<QWidget *>(parent()));
    return window;
  }


  /**
   * Called when the user has finished drawing with the rubber
   * band.  ChangePlot is called to plot the data within the
   * rubber band.
   */
  void EphemeridesPlotTool::rubberBandComplete() {
    if (selectedWindow()) {
      selectedWindow()->raise();
    }

    refreshPlot();
  }


  /**
   * This method plots the selected cube's data in an ephemerides window.
   */
  void EphemeridesPlotTool::refreshPlot() {
    MdiCubeViewport *activeViewport = cubeViewport();

    if (activeViewport) {
      EphemeridesPlotWindow *targetWindow = qobject_cast<EphemeridesPlotWindow *>(
          selectedWindow(true));

      Cube *cube = activeViewport->cube();
      std::vector<double> coordinateTimes, xCoordinates, yCoordinates, zCoordinates,
                          angleTimes, raAngles, decAngles, twiAngles;
      collectEphemerides(cube, coordinateTimes, xCoordinates, yCoordinates, zCoordinates,
                         angleTimes, raAngles, decAngles, twiAngles);

      validatePlotCurves();

      // Transfer data to the plotcurves
      m_xCurve->setSamples(&coordinateTimes[0], &xCoordinates[0], coordinateTimes.size());
      m_yCurve->setSamples(&coordinateTimes[0], &yCoordinates[0], coordinateTimes.size());
      m_zCurve->setSamples(&coordinateTimes[0], &zCoordinates[0], coordinateTimes.size());
      m_raCurve->setSamples(&angleTimes[0],     &raAngles[0],     angleTimes.size());
      m_decCurve->setSamples(&angleTimes[0],    &decAngles[0],    angleTimes.size());
      m_twiCurve->setSamples(&angleTimes[0],    &twiAngles[0],    angleTimes.size());

      // Get the corners of the cube
      QList<QPoint> vertices;
      vertices.append(QPoint(0,0));
      vertices.append(QPoint(0,0));
      vertices.append(QPoint(0,0));
      vertices.append(QPoint(0,0));
      activeViewport->cubeToViewport(0.5,                       0.5,
                                     vertices[0].rx(),          vertices[0].ry());
      activeViewport->cubeToViewport(0.5,                       cube->lineCount() + 0.5,
                                     vertices[1].rx(),          vertices[1].ry());
      activeViewport->cubeToViewport(cube->sampleCount() + 0.5, cube->lineCount() + 0.5,
                                     vertices[2].rx(),          vertices[2].ry());
      activeViewport->cubeToViewport(cube->sampleCount() + 0.5, 0.5,
                                     vertices[3].rx(),          vertices[3].ry());
      
      m_xCurve->setSource(activeViewport, vertices);
      m_yCurve->setSource(activeViewport, vertices);
      m_zCurve->setSource(activeViewport, vertices);
      m_raCurve->setSource(activeViewport, vertices);
      m_decCurve->setSource(activeViewport, vertices);
      m_twiCurve->setSource(activeViewport, vertices);

      targetWindow->replot();
    }
  }


  /**
   * This method sets up the names, line style, and color  of the
   * all the plot items that will be used in this class.
   */
  void EphemeridesPlotTool::validatePlotCurves() {
      EphemeridesPlotWindow *targetWindow = qobject_cast<EphemeridesPlotWindow *>(
          selectedWindow());

    if (targetWindow) {

      QPen positionPen(Qt::red);
      positionPen.setWidth(2);

      QPen rotationPen(Qt::darkCyan);
      positionPen.setWidth(2);

      if (!m_xCurve) {
        m_xCurve = createCurve("X Coordinate", positionPen,
            CubePlotCurve::EphemerisTime, CubePlotCurve::Kilometers);
        m_xCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        targetWindow->add(m_xCurve);
      }

      if (!m_yCurve) {
        m_yCurve = createCurve("Y Coordinate", positionPen,
            CubePlotCurve::EphemerisTime, CubePlotCurve::Kilometers);
        m_yCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        targetWindow->add(m_yCurve);
      }

      if (!m_zCurve) {
        m_zCurve = createCurve("Z Coordinate", positionPen,
            CubePlotCurve::EphemerisTime, CubePlotCurve::Kilometers);
        m_zCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        targetWindow->add(m_zCurve);
      }

      if (!m_raCurve) {
        m_raCurve = createCurve("Right Ascension Angle", rotationPen,
            CubePlotCurve::EphemerisTime, CubePlotCurve::Radians);
        m_raCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        m_raCurve->setYAxis(QwtPlot::yRight);
        targetWindow->addRotation(m_raCurve);
      }

      if (!m_decCurve) {
        m_decCurve = createCurve("Declination Angle", rotationPen,
            CubePlotCurve::EphemerisTime, CubePlotCurve::Radians);
        m_decCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        m_decCurve->setYAxis(QwtPlot::yRight);
        targetWindow->addRotation(m_decCurve);
      }

      if (!m_twiCurve) {
        m_twiCurve = createCurve("Twist Angle", rotationPen,
            CubePlotCurve::EphemerisTime, CubePlotCurve::Radians);
        m_twiCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        m_twiCurve->setYAxis(QwtPlot::yRight);
        targetWindow->addRotation(m_twiCurve);
      }
    }
  }


  /**
   * Collect ephemeride data from a cube.
   * 
   * @param cube A pointer to the cube to collect data from
   * 
   * @param[out] coordinateTimes A vector containing the times for the coordinate data.
   * @param[out] xCoordinates A vector containing x coordinate data.
   * @param[out] yCoordinates A vector containing y coordinate data.
   * @param[out] zCoordinates A vector containing z coordinate data.
   * @param[out] angleTimes A vector containing the times for the angle data.
   * @param[out] raAngles A vector containing right ascension angle data
   * @param[out] decAngles A vector containing declination angle data
   * @param[out] twistAngles A vector containing twist angle data
   */
  void EphemeridesPlotTool::collectEphemerides(Cube *cube,
                                               std::vector<double> &coordinateTimes,
                                               std::vector<double> &xCoordinates,
                                               std::vector<double> &yCoordinates,
                                               std::vector<double> &zCoordinates,
                                               std::vector<double> &angleTimes,
                                               std::vector<double> &raAngles,
                                               std::vector<double> &decAngles,
                                               std::vector<double> &twiAngles) {
    // Get the instrument position and rotation for the cube
    Camera *cam = cube->camera();
    SpicePosition *instPosition = cam->instrumentPosition();
    SpiceRotation *instRotation = cam->instrumentRotation();

    // Clear the output vectors
    coordinateTimes.clear();
    xCoordinates.clear();
    yCoordinates.clear();
    zCoordinates.clear();
    angleTimes.clear();
    raAngles.clear();
    decAngles.clear();
    twiAngles.clear();

    // Get the camera start and end times
    std::pair<double, double> startEndTimes = cam->StartEndEphemerisTimes();

    // Collect the coordinate times
    // If there is a cache, use the cache times
    if ( instPosition->IsCached() ) {
      coordinateTimes = instPosition->timeCache();
      // If there's only one cached point, then use the camera's start, end time to draw a line
      if ( coordinateTimes.size() < 2 ) {
        coordinateTimes.clear();
        coordinateTimes.push_back(startEndTimes.first);
        coordinateTimes.push_back(startEndTimes.second);
      }
    }
    // If there is no cache to work with, pick times to evaluate the spice at
    else {
      int sampleCount = 100;
      double sampleRate = ( startEndTimes.second - startEndTimes.first ) / sampleCount;
      for (int i = 0; i < sampleCount; i++) {
        coordinateTimes.push_back(startEndTimes.first + sampleRate * i);
      }
    }

    // Collect the angle times
    // If there is a cache, use the cache times
    if ( instRotation->IsCached() ) {
      angleTimes = instRotation->timeCache();
      // If there's only one cached point, then use the camera's start, end time to draw a line
      if ( angleTimes.size() < 2 ) {
        angleTimes.clear();
        angleTimes.push_back(startEndTimes.first);
        angleTimes.push_back(startEndTimes.second);
      }
    }
    // If there is no cache to work with, pick times to evaluate the spice at
    else {
      int sampleCount = 100;
      double sampleRate = ( startEndTimes.second - startEndTimes.first ) / sampleCount;
      for (int i = 0; i < sampleCount; i++) {
        angleTimes.push_back(startEndTimes.first + sampleRate * i);
      }
    }

    // Collect the coordinates
    for (int i = 0; i < (int)coordinateTimes.size(); i++) {
      std::vector<double> coordinate = instPosition->SetEphemerisTime(coordinateTimes[i]);
      xCoordinates.push_back(coordinate[0]);
      yCoordinates.push_back(coordinate[1]);
      zCoordinates.push_back(coordinate[2]);
    }

    // Collect the angles
    double start1 = 0.; // value of 1st angle1 in cache
    double start3 = 0.; // value of 1st angle3 in cache
    for (int i = 0; i < (int)angleTimes.size(); i++) {
      instRotation->SetEphemerisTime(angleTimes[i]);
      // This assumes that the euler angles are 3, 1, 3 which matches how
      // polynomials are fit to them.
      std::vector<double> angles = instRotation->Angles(3, 1, 3);
      // Fix angles crossing over the domain bound
      if (i == 0) {
        start1 = angles[0];
        start3 = angles[2];
      }
      else {
        angles[0] = instRotation->WrapAngle(start1, angles[0]);
        angles[2] = instRotation->WrapAngle(start3, angles[2]);
      }
      raAngles.push_back(angles[0]);
      decAngles.push_back(angles[1]);
      twiAngles.push_back(angles[2]);
    }
  }
}

