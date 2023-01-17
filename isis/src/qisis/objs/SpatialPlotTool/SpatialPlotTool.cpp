#include "SpatialPlotTool.h"

#include <iostream>

#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Point.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QStackedWidget>

#include "Brick.h"
#include "Camera.h"
#include "Cube.h"
#include "CubePlotCurve.h"
#include "Distance.h"
#include "InterestOperator.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MdiCubeViewport.h"
#include "PlotWindow.h"
#include "PolygonTools.h"
#include "Projection.h"
#include "RingPlaneProjection.h"
#include "TProjection.h"
#include "Pvl.h"
#include "RubberBandComboBox.h"
#include "RubberBandTool.h"
#include "Statistics.h"
#include "SurfacePoint.h"
#include "ToolPad.h"
#include "UniversalGroundMap.h"

using std::cerr;

namespace Isis {
  /**
   * Create a spatial plot tool.
   *
   * @param parent The Qt-parent relationship parent widget
   */
  SpatialPlotTool::SpatialPlotTool(QWidget *parent) : AbstractPlotTool(parent),
      m_spatialCurves(new QMap<MdiCubeViewport *, QPointer<CubePlotCurve> >) {
    //connect(m_toolPadAction, SIGNAL(activated()), this, SLOT(showPlotWindow()));
    connect(this, SIGNAL(viewportChanged()), this, SLOT(viewportSelected()));

    m_xUnitsCombo = new QComboBox;
  }


  /**
   * This protected slot is called when user selects a viewport.
   *
   */
  void SpatialPlotTool::viewportSelected() {
  }


  /**
   * This method is called when the tool is activated by the
   *   parent, or when the plot mode is changed. It's used to
   *   activate or change the rubber banding mode to be either
   *   rectangle or line, depending on the current plot type.
   */
  void SpatialPlotTool::enableRubberBandTool() {
    m_rubberBandCombo->reset();
    m_rubberBandCombo->setVisible(true);
    m_rubberBandCombo->setEnabled(true);
    rubberBandTool()->setDrawActiveViewportOnly(false);
  }


  /**
   * This method configures the QAction for this tool
   *
   * @param toolpad - the ToolPad to add the SpatialPlotTool to
   *
   * @return QAction* - the QAction that was created for this tool
   */
  QAction *SpatialPlotTool::toolPadAction(ToolPad *toolpad) {
    m_toolPadAction = new QAction(toolpad);
    m_toolPadAction->setText("Spatial Plot Tool");
    m_toolPadAction->setIcon(QPixmap(toolIconDir() + "/spatial_plot.png"));
    QString text = "<b>Function:</b> Create a spatial plot of the selected pixels' DN values.";
    m_toolPadAction->setWhatsThis(text);
    return m_toolPadAction;
  }


  /**
   * Creates the widgets for the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *SpatialPlotTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *wrapper = new QWidget(parent);

    m_rubberBandCombo = new RubberBandComboBox(this,
      RubberBandComboBox::Line |
      RubberBandComboBox::RotatedRectangle,
      RubberBandComboBox::Line,
      true
    );

    m_interpolationCombo = new QComboBox;
    m_interpolationCombo->addItem("Nearest Neighbor",
        Interpolator::NearestNeighborType);
    m_interpolationCombo->addItem("BiLinear",
        Interpolator::BiLinearType);
    m_interpolationCombo->addItem("Cubic Convolution",
        Interpolator::CubicConvolutionType);
    m_interpolationCombo->setCurrentIndex(
        m_interpolationCombo->findText("Nearest Neighbor"));
    connect(m_interpolationCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(refreshPlot()));

    QWidget *abstractToolWidgets =
        AbstractPlotTool::createToolBarWidget(parent);

    QHBoxLayout *layout = new QHBoxLayout(wrapper);
    layout->setMargin(0);
    layout->addWidget(m_rubberBandCombo);
    layout->addWidget(new QLabel("Interpolation:"));
    layout->addWidget(m_interpolationCombo);
    layout->addWidget(abstractToolWidgets);
    layout->addWidget(m_xUnitsCombo);
    layout->addStretch(1);
    wrapper->setLayout(layout);

    return wrapper;
  }


  /**
   * Updates plot tool.
   *
   */
  void SpatialPlotTool::updateTool() {
    AbstractPlotTool::updateTool();

    PlotCurve::Units preferredUnits =
        (PlotCurve::Units)m_xUnitsCombo->itemData(
          m_xUnitsCombo->currentIndex()).toInt();

    while (m_xUnitsCombo->count())
      m_xUnitsCombo->removeItem(0);

    m_xUnitsCombo->addItem("Pixel Number", PlotCurve::PixelNumber);

    bool haveGroundMaps = true;
    foreach (MdiCubeViewport *cvp, viewportsToPlot()) {
      haveGroundMaps = haveGroundMaps && cvp->universalGroundMap();
    }

    if (haveGroundMaps) {
      m_xUnitsCombo->addItem("Meters", PlotCurve::Meters);
      m_xUnitsCombo->addItem("Kilometers", PlotCurve::Kilometers);
    }

    if (m_xUnitsCombo->findData(preferredUnits) != -1) {
      m_xUnitsCombo->setCurrentIndex(
        m_xUnitsCombo->findData(preferredUnits));
    }

    m_xUnitsCombo->setVisible(m_xUnitsCombo->count() > 1);
  }


  /**
   * Creates a new plot window compatible with the curves in this tool.
   *
   * @return a newly allocated plot window, ownership is passed to the caller.
   */
  PlotWindow *SpatialPlotTool::createWindow() {
    PlotWindow *window = new PlotWindow(
        "Spatial " + PlotWindow::defaultWindowTitle(),
          (PlotCurve::Units)m_xUnitsCombo->itemData(
            m_xUnitsCombo->currentIndex()).toInt(),
        PlotCurve::CubeDN, qobject_cast<QWidget *>(parent()));
    return window;
  }


  /**
   * Forget about all existing spatial plot curves. Don't delete them, just
   *   forget them so that when the user requests a new one they get a brand
   *   new curve.
   */
  void SpatialPlotTool::detachCurves() {
    m_spatialCurves->clear();
  }


  /**
   * Called when the user has finished drawing with the rubber
   * band.  ChangePlot is called to plot the data within the
   * rubber band.
   *
   */
  void SpatialPlotTool::rubberBandComplete() {
    if (selectedWindow()) {
      selectedWindow()->raise();
    }

    if (rubberBandTool()->isValid()) {
      refreshPlot();
    }
    else {
      QMessageBox::information(NULL, "Error",
                               "The selected Area contains no valid pixels",
                               QMessageBox::Ok);
    }
  }


  /**
   * This method replots the data, with current settings and rubber band,
   * in the plot window.
   */
  void SpatialPlotTool::refreshPlot() {
    MdiCubeViewport *activeViewport = cubeViewport();

    if (activeViewport && rubberBandTool()->isValid()) {
      // Find which window we want to paste into
      PlotWindow *targetWindow = selectedWindow(true);

      // if the selected window won't work, create a new one
      if (targetWindow->xAxisUnits() !=
          m_xUnitsCombo->itemData(m_xUnitsCombo->currentIndex()).toInt()) {
        targetWindow = addWindow();
      }

      // get curves for active viewport and also for any linked viewports
      foreach (MdiCubeViewport *viewport, viewportsToPlot()) {
        QVector<QPointF> data = getSpatialStatistics(viewport);

        // load data into curve
        if (data.size() > 0) {
          QList<QPoint> rubberBandPoints = rubberBandTool()->vertices();

          validatePlotCurves();
          int band = ((viewport->isGray()) ? viewport->grayBand() :
                                             viewport->redBand());
          (*m_spatialCurves)[viewport]->setData(new QwtPointSeriesData(data));
          (*m_spatialCurves)[viewport]->setSource(
              viewport, rubberBandPoints, band);
        }
      }

      targetWindow->replot();
      updateTool();
    }
  }


  /**
   * This method sets up the names, line style, and color  of the
   * all the CubePlotCurves that will be used in this class.
   */
  void SpatialPlotTool::validatePlotCurves() {
    PlotWindow *targetWindow = selectedWindow();

    if (targetWindow) {
      PlotCurve::Units targetUnits = (PlotCurve::Units)m_xUnitsCombo->itemData(
            m_xUnitsCombo->currentIndex()).toInt();

      QPen spatialPen(Qt::white);
      spatialPen.setWidth(1);
      spatialPen.setStyle(Qt::SolidLine);

      foreach (MdiCubeViewport *viewport, viewportsToPlot()) {
        if (!(*m_spatialCurves)[viewport] ||
            (*m_spatialCurves)[viewport]->xUnits() != targetUnits) {
          CubePlotCurve *plotCurve = createCurve("DN Values", spatialPen,
              targetUnits, CubePlotCurve::CubeDN);
          m_spatialCurves->insert(viewport, plotCurve);
          targetWindow->add(plotCurve);
        }
      }
    }
  }


  SurfacePoint SpatialPlotTool::resultToSurfacePoint(UniversalGroundMap *groundMap) {
    SurfacePoint result;

    if (groundMap) {
      Distance radius;

      if (groundMap->Camera())
        radius = groundMap->Camera()->LocalRadius();
      else if (groundMap->Projection())
        radius = Distance(groundMap->Projection()->LocalRadius(), Distance::Meters);

      result = SurfacePoint(Latitude(groundMap->UniversalLatitude(), Angle::Degrees),
          Longitude(groundMap->UniversalLongitude(), Angle::Degrees), radius);
    }

    return result;
  }


  /**
   *
   *
   * @param labels
   * @param data
   * @param cvp
   */
  QVector<QPointF> SpatialPlotTool::getSpatialStatistics(MdiCubeViewport *cvp) {
    QList<QPoint> vertices = rubberBandTool()->vertices();

    QVector<QPointF> data;

    PlotCurve::Units targetUnits = (PlotCurve::Units)m_xUnitsCombo->itemData(
          m_xUnitsCombo->currentIndex()).toInt();

    if (cvp && vertices.size()) {
      Interpolator interp;
      interp.SetType(
          (Isis::Interpolator::interpType) m_interpolationCombo->itemData(
            m_interpolationCombo->currentIndex()).toInt());

      Portal dataReader(interp.Samples(), interp.Lines(),
                        cvp->cube()->pixelType());

      int band = ((cvp->isGray()) ? cvp->grayBand() : cvp->redBand());

      if (rubberBandTool()->currentMode() == RubberBandTool::LineMode) {
        double startSample = Null;
        double endSample = Null;
        double startLine = Null;
        double endLine = Null;

        cvp->viewportToCube(vertices[0].x(), vertices[0].y(),
                            startSample, startLine);
        cvp->viewportToCube(vertices[1].x(), vertices[1].y(),
                            endSample, endLine);

        // round to the nearest pixel increment
        int lineLength = qRound(sqrt(pow(startSample - endSample, 2) +
                                     pow(startLine - endLine, 2)));

        SurfacePoint startPoint;
        UniversalGroundMap *groundMap = cvp->universalGroundMap();
        if (targetUnits != PlotCurve::PixelNumber) {
          if (groundMap->SetImage(startSample, startLine)) {
            startPoint = resultToSurfacePoint(groundMap);
          }
          else {
            QMessageBox::warning(qobject_cast<QWidget *>(parent()),
                tr("Failed to project points along line"),
                tr("Failed to project (calculate a latitude, longitude, and radius) for the "
                   "starting point of the line (sample [%1], line [%2]).")
                  .arg(startSample).arg(startLine));
            return data;
          }
        }

        if (lineLength > 0) {
          for(int index = 0; index <= lineLength; index++) {
            // % across * delta x + initial = x position of point (sample)
            double sample = (index / (double)lineLength) * (endSample - startSample) +
                       startSample;
            // move back for interpolation
            sample -= (interp.Samples() / 2.0 - 0.5);

            double line = (index / (double)lineLength) * (endLine - startLine) +
                       startLine;
            line -= (interp.Lines() / 2.0 - 0.5);

            dataReader.SetPosition(sample, line, band);
            cvp->cube()->read(dataReader);

            double result = interp.Interpolate(sample + 0.5, line + 0.5, dataReader.DoubleBuffer());

            if (!IsSpecial(result)) {
              double plotXValue = index + 1;

              if (targetUnits != PlotCurve::PixelNumber) {
                plotXValue = sample;
 
                if (groundMap->SetImage(sample, line)) {
                  Distance xDistance = startPoint.GetDistanceToPoint(resultToSurfacePoint(groundMap));

                  if (targetUnits == PlotCurve::Meters)
                    plotXValue = xDistance.meters();
                  else if (targetUnits == PlotCurve::Kilometers)
                    plotXValue = xDistance.kilometers();
                }
                else {
                  QMessageBox::warning(qobject_cast<QWidget *>(parent()),
                      tr("Failed to project points along line"),
                      tr("Failed to project (calculate a latitude, longitude, and radius) for a "
                         "point along the line (sample [%1], line [%2]).")
                        .arg(startSample).arg(startLine));
                  return data;
                }
              }  

              data.append(QPointF(plotXValue, result));
            }
          }
        }
        else {
          QMessageBox::information(NULL, "Error",
                                   "The selected Area contains no valid pixels",
                                   QMessageBox::Ok);
        }
      }
      else if (rubberBandTool()->currentMode() == RubberBandTool::RotatedRectangleMode) {
        /*
         * We have a rotated rectangle:
         *
         *    --acrossLength-->
         *  --------------------
         *  |A                B|
         *  |                  | |
         *  |                  | |
         *  |                  | |
         *  |                  | |
         *  |                  | | rectangleLength
         *  |                  | |
         *  |                  | |
         *  |                  | |
         *  |                  | |
         *  |                  | V
         *  |D                C|
         *  -------------------
         *
         * A is the point where the user initially clicked to start drawing the
         * rectangle. A is clickSample, clickLine.
         *
         * B is the initial mouse release that defines the width and direction
         * of the rectangle. B is acrossSample, acrossLine.
         *
         * C is not needed for our calculations.
         *
         * D is endSample, endLine.
         */
        double clickSample = Null;
        double clickLine = Null;
        double acrossSample = Null;
        double acrossLine = Null;
        double endSample = Null;
        double endLine = Null;

        cvp->viewportToCube(vertices[0].x(), vertices[0].y(),
                            clickSample, clickLine);
        cvp->viewportToCube(vertices[1].x(), vertices[1].y(),
                            acrossSample, acrossLine);
        cvp->viewportToCube(vertices[3].x(), vertices[3].y(),
                            endSample, endLine);

        double acrossVectorX = acrossSample - clickSample;
        double acrossVectorY = acrossLine - clickLine;

        // Get length of "green" line on the screen
        int acrossLength = qRound(sqrt(acrossVectorX * acrossVectorX +
                                       acrossVectorY * acrossVectorY));
        
        double sampleStepAcross = (1.0 / (double)acrossLength) * acrossVectorX;
        double lineStepAcross = (1.0 / (double)acrossLength) * acrossVectorY;

        double lengthVectorSample = endSample - clickSample;
        double lengthVectorLine = endLine - clickLine;
        
        // Get length of "red" line on the screen
        int rectangleLength = qRound(sqrt(lengthVectorSample * lengthVectorSample +
                                          lengthVectorLine * lengthVectorLine));
                                          
        // Prevent length of zero for later calculations
        if (rectangleLength == 0) {
          rectangleLength = 1;
        }

        SurfacePoint startPoint;
        UniversalGroundMap *groundMap = cvp->universalGroundMap();
        if (targetUnits != PlotCurve::PixelNumber) {
          double midStartSample = (clickSample + acrossSample) / 2.0;
          double midStartLine = (clickLine + acrossLine) / 2.0;
          if (groundMap->SetImage(midStartSample, midStartLine)) {
            startPoint = resultToSurfacePoint(groundMap);
          }
          else {
            QMessageBox::warning(qobject_cast<QWidget *>(parent()),
                tr("Failed to project points along line"),
                tr("Failed to project (calculate a latitude, longitude, and radius) for the "
                   "starting point of the line (sample [%1], line [%2]).")
                  .arg(midStartSample).arg(midStartLine));
            return data;
          }
        }

        // walk the "red" line on the screen
        for(int index = 0; index <= rectangleLength; index++) {
          Statistics acrossStats;

          // % along length * lengthVectorSample + clickSample = x position of point
          double sample = (index / (double)rectangleLength) * lengthVectorSample +
                     clickSample;
          // move back for interpolation
          sample -= (interp.Samples() / 2.0 - 0.5);

          double line = (index / (double)rectangleLength) * lengthVectorLine +
                     clickLine;
          line -= (interp.Lines() / 2.0 - 0.5);

          double sampleMid = sample + (acrossLength / 2.0) * sampleStepAcross;
          double lineMid = line + (acrossLength / 2.0) * lineStepAcross;

          // For each pixel length in the red line direction, we are now recursing through each 
          // pixel length in the green line's direction and adding the pixel values
          for(int acrossPixel = 0;
              acrossPixel <= acrossLength;
              acrossPixel++) {
            dataReader.SetPosition(sample, line, band);
            cvp->cube()->read(dataReader);
            double pixelValue = interp.Interpolate(sample + 0.5, line + 0.5,
                                                   dataReader.DoubleBuffer());

            if (!IsSpecial(pixelValue)) {
              acrossStats.AddData(pixelValue);
            }

            sample += sampleStepAcross;
            line += lineStepAcross;
          }

          if (!IsSpecial(acrossStats.Average())) {
            double plotXValue = index + 1;

            if (targetUnits != PlotCurve::PixelNumber) {
              if (groundMap->SetImage(sampleMid, lineMid)) {
                Distance xDistance = startPoint.GetDistanceToPoint(resultToSurfacePoint(groundMap));

                if (targetUnits == PlotCurve::Meters)
                  plotXValue = xDistance.meters();
                else if (targetUnits == PlotCurve::Kilometers)
                  plotXValue = xDistance.kilometers();
              }
              else {
                QMessageBox::warning(qobject_cast<QWidget *>(parent()),
                    tr("Failed to project points along line"),
                    tr("Failed to project (calculate a latitude, longitude, and radius) for a "
                       "point along the line (sample [%1], line [%2]).")
                      .arg(sampleMid).arg(lineMid));
                return data;
              }
            }

            data.append(QPointF(plotXValue, acrossStats.Average()));
          }
        }
      }
    }

    return data;
  }
}

