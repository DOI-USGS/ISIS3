#include "IsisDebug.h"

#include "SpatialPlotTool.h"

#include <iostream>

#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Point.h>

#include <QHBoxLayout>
#include <QMenu>
#include <QStackedWidget>

#include "Brick.h"
#include "Cube.h"
#include "CubePlotCurve.h"
#include "InterestOperator.h"
#include "MdiCubeViewport.h"
#include "PlotWindow.h"
#include "PolygonTools.h"
#include "Pvl.h"
#include "RubberBandComboBox.h"
#include "RubberBandTool.h"
#include "Statistics.h"
#include "ToolPad.h"

using std::cerr;

namespace Isis {

  SpatialPlotTool::SpatialPlotTool(QWidget *parent) : AbstractPlotTool(parent),
      m_spatialCurves(new QMap<MdiCubeViewport *, QPointer<CubePlotCurve> >) {
    m_toolPadAction = new QAction(this);
    m_toolPadAction->setText("Spatial Plot Tool");
    m_toolPadAction->setIcon(QPixmap(toolIconDir() + "/spatial_plot.png"));
    //connect(m_toolPadAction, SIGNAL(activated()), this, SLOT(showPlotWindow()));
    connect(this, SIGNAL(viewportChanged()), this, SLOT(viewportSelected()));
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
  }


  /**
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *SpatialPlotTool::toolPadAction(ToolPad *toolpad) {
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

    m_rubberBandCombo = new RubberBandComboBox(
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
        m_interpolationCombo->findText("BiLinear"));
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
  }


  /**
   * Creates the active plot window
   *
   */
  PlotWindow *SpatialPlotTool::createWindow() {
    PlotWindow *window = new PlotWindow(
        "Spatial " + PlotWindow::defaultWindowTitle(), PlotCurve::PixelNumber,
        PlotCurve::CubeDN, qobject_cast<QWidget *>(parent()));
    return window;
  }


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

    if(RubberBandTool::isValid()) {
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

    if (activeViewport && RubberBandTool::isValid()) {
      // Find which window we want to paste into
      PlotWindow *targetWindow = selectedWindow(true);

      // get curves for active viewport and also for any linked viewports
      foreach (MdiCubeViewport *viewport, viewportsToPlot()) {
        QVector<double> labels;
        QVector<double> dnValues;
        getSpatialStatistics(labels, dnValues, viewport);

        // load data into curve
        ASSERT(labels.size() == dnValues.size());
        if (labels.size() > 0) {
          QList<QPoint> rubberBandPoints = RubberBandTool::getVertices();

          validatePlotCurves();
          int band = ((viewport->isGray()) ? viewport->grayBand() :
                                             viewport->redBand());
          (*m_spatialCurves)[viewport]->setData(
              &labels[0], &dnValues[0], labels.size());
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
      QPen spatialPen(Qt::white);
      spatialPen.setWidth(2);

      foreach (MdiCubeViewport *viewport, viewportsToPlot()) {
        if (!(*m_spatialCurves)[viewport]) {
          CubePlotCurve *plotCurve = createCurve("DN Values", spatialPen,
              CubePlotCurve::PixelNumber, CubePlotCurve::CubeDN);
          m_spatialCurves->insert(viewport, plotCurve);
          targetWindow->add(plotCurve);
        }
      }
    }
  }


  double SpatialPlotTool::testSpecial(double pixel) {
    return (IsSpecial(pixel)) ? 0.0 : pixel;
  }


  /**
   *
   *
   * @param labels
   * @param data
   */
  void SpatialPlotTool::getSpatialStatistics(QVector<double> &labels,
                                             QVector<double> &data,
                                             MdiCubeViewport * cvp) {
    QList<QPoint> vertices = RubberBandTool::getVertices();

    if(cvp && vertices.size()) {
      Interpolator interp;
      interp.SetType(
          (Isis::Interpolator::interpType) m_interpolationCombo->itemData(
            m_interpolationCombo->currentIndex()).toInt());

      Portal dataReader(interp.Samples(), interp.Lines(),
                        cvp->cube()->getPixelType());

      int band = ((cvp->isGray()) ? cvp->grayBand() : cvp->redBand());

      if(RubberBandTool::getMode() == RubberBandTool::Line) {
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
        for(int index = 0; index < lineLength; index++) {
          // % across * delta x + initial = x position of point
          double x = (index / (double)lineLength) * (endSample - startSample) +
                     startSample;
          // move back for interpolation
          x -= (interp.Samples() / 2.0);

          double y = (index / (double)lineLength) * (endLine - startLine) +
                     startLine;
          y -= (interp.Lines() / 2.0);

          dataReader.SetPosition(x, y, band);
          cvp->cube()->read(dataReader);
          double result = interp.Interpolate(x, y, dataReader.DoubleBuffer());

          if(!IsSpecial(result)) {
            labels.push_back(index + 1);
            data.push_back(result);
          }
        }
      }
      else if(RubberBandTool::getMode() == RubberBandTool::RotatedRectangle) {
        /*
         * We have a rotated rectangle:
         *
         *    --across-->
         *  --------------
         *  |A          B|
         *  |            | |
         *  |            | |
         *  |            | |
         *  |            | |
         *  |            | | length
         *  |            | |
         *  |            | |
         *  |            | |
         *  |            | |
         *  |            | V
         *  |D          C|
         *  --------------
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

        int numStepsAcross = qRound(sqrt(acrossVectorX * acrossVectorX +
                                         acrossVectorY * acrossVectorY));
        double xStepAcross = (1.0 / (double)numStepsAcross) * acrossVectorX;
        double yStepAcross = (1.0 / (double)numStepsAcross) * acrossVectorY;

        double lengthVectorX = endSample - clickSample;
        double lengthVectorY = endLine - clickLine;
        int rectangleLength = qRound(sqrt(lengthVectorX * lengthVectorX +
                                          lengthVectorY * lengthVectorY));

        // walk the "green" line on the screen
        for(int index = 0; index < rectangleLength; index++) {
          Statistics acrossStats;

          // % along length * lengthVectorX + clickSample = x position of point
          double x = (index / (double)rectangleLength) * lengthVectorX +
                     clickSample;
          // move back for interpolation
          x -= (interp.Samples() / 2.0);

          double y = (index / (double)rectangleLength) * lengthVectorY +
                     clickLine;
          y -= (interp.Lines() / 2.0);

          for(int acrossPixel = 0;
              acrossPixel < numStepsAcross;
              acrossPixel++) {
            dataReader.SetPosition(x, y, band);
            cvp->cube()->read(dataReader);
            double pixelValue = interp.Interpolate(x, y,
                                                   dataReader.DoubleBuffer());

            if(!IsSpecial(pixelValue)) {
              acrossStats.AddData(pixelValue);
            }

            x += xStepAcross;
            y += yStepAcross;
          }

          if(!IsSpecial(acrossStats.Average())) {
            labels.push_back(index + 1);
            data.push_back(acrossStats.Average());
          }
        }
      }
    }
  }
}

