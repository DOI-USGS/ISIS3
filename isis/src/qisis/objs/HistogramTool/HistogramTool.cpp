#include "HistogramTool.h"

#include <geos/geom/Point.h>

#include <QDebug>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "Brick.h"
#include "CubePlotCurve.h"
#include "ImageHistogram.h"
#include "Histogram.h"
#include "HistogramItem.h"
#include "HistogramPlotWindow.h"
#include "MdiCubeViewport.h"
#include "PolygonTools.h"
#include "RubberBandComboBox.h"
#include "ToolPad.h"


namespace Isis {

  /**
   * Constructor creates a new HistogramTool object.
   *
   * @param parent
   */
  HistogramTool::HistogramTool(QWidget *parent) : AbstractPlotTool(parent) {
    m_action = new QAction(this);
    m_action->setText("Histogram Tool");
    m_action->setIcon(QPixmap(toolIconDir() + "/histogram.png"));
  }



  /**
   * This method is called when the tool is activated by the
   *   parent, or when the plot mode is changed. It's used to
   *   activate or change the rubber banding mode to be either
   *   rectangle or line, depending on the current plot type.
   */
  void HistogramTool::enableRubberBandTool() {
    if(m_rubberBandCombo) {
      m_rubberBandCombo->reset();
      m_rubberBandCombo->setEnabled(true);
      rubberBandTool()->setDrawActiveViewportOnly(true);
    }
  }

  /**
   * This method adds the histogram tool to the tool pad.
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *HistogramTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir() + "/histogram.png"));
    action->setToolTip("Histogram (H)");
    action->setShortcut(Qt::Key_H);

    QString text  =
      "<b>Function:</b>  Plot histogram in active viewport \
      <p><b>Shortcut:</b> H</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * This method creates the widgets for the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *HistogramTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *wrapper = new QWidget;

    m_rubberBandCombo = new RubberBandComboBox(this,
      RubberBandComboBox::Rectangle|
      RubberBandComboBox::Line,
      RubberBandComboBox::Rectangle
    );

    QWidget *abstractToolWidgets =
        AbstractPlotTool::createToolBarWidget(parent);

    QHBoxLayout *layout = new QHBoxLayout(wrapper);
    layout->setMargin(0);
    layout->addWidget(m_rubberBandCombo);
    layout->addWidget(abstractToolWidgets);
    layout->addStretch(1);
    wrapper->setLayout(layout);

    return wrapper;
  }


  /**
   * Forget the frequency histogram item and the percentage curve.
   */
  void HistogramTool::detachCurves() {
    m_frequencyItem = NULL;
    m_percentageCurve = NULL;
  }


  /**
   * This method updates the histogram tool.
   *
   */
  void HistogramTool::updateTool() {
    AbstractPlotTool::updateTool();
  }


  /**
   * This method creates the default histogram plot window.
   *
   */
  PlotWindow *HistogramTool::createWindow() {
    PlotWindow *window = new HistogramPlotWindow(
        "Histogram " + PlotWindow::defaultWindowTitle(),
        qobject_cast<QWidget *>(parent()));
    return window;
  }


  /**
   * Called when the user has finished drawing with the rubber
   * band.  ChangePlot is called to plot the data within the
   * rubber band.
   *
   */
  void HistogramTool::rubberBandComplete() {
    if (selectedWindow()) {
      selectedWindow()->raise();
    }

    if(rubberBandTool()->isValid()) {
      if (cubeViewport()->isGray()) {
        refreshPlot();
      }
      else {
        QMessageBox::information(NULL, "Error",
                                 "Cannot create histogram on colored viewport",
                                 QMessageBox::Ok);
      }
    }
    else {
      QMessageBox::information(NULL, "Error",
                               "The selected Area contains no valid pixels",
                               QMessageBox::Ok);
    }
  }


  /**
   * This method plots the selected data in a histogram window.
   */
  void HistogramTool::refreshPlot() {
    MdiCubeViewport *activeViewport = cubeViewport();

    if (activeViewport && rubberBandTool()->isValid()) {
      HistogramPlotWindow *targetWindow = qobject_cast<HistogramPlotWindow *>(
          selectedWindow(true));

      QList<QPoint> vertices;

      if(rubberBandTool()->currentMode() == RubberBandTool::CircleMode) {
        geos::geom::Geometry *p = rubberBandTool()->geometry();
        geos::geom::CoordinateSequence *c = p->getCoordinates().release();
        for(int i = 0; i < (int)c->getSize(); i++) {
          QPoint point((int)(c->getX(i) + 0.5), (int)(c->getY(i) + 0.5));
          vertices.append(point);
        }
        delete p;
      }
      else {
        vertices = rubberBandTool()->vertices();
      }

      if(vertices.size() < 1) return;

      Cube *cube = activeViewport->cube();
      int band = activeViewport->grayBand();
      ImageHistogram hist(*cube, band);

      //If the rubber band is a line
      if (rubberBandTool()->currentMode() == RubberBandTool::LineMode) {
        double ssamp, sline, esamp, eline;
        activeViewport->viewportToCube(vertices[0].rx(), vertices[0].ry(),
                            ssamp, sline);

        activeViewport->viewportToCube(vertices[1].rx(), vertices[1].ry(),
                            esamp, eline);

        QLine line((int)ssamp, (int)sline, (int)esamp, (int)eline);

        double slope;
        int i;
        int x, y, xinc, yinc;
        int xsize, ysize;

        //Get all of the points out of the line
        QList<QPoint *> *linePts = new QList<QPoint *>;

        int sx = line.p1().x();
        int ex = line.p2().x();
        int sy = line.p1().y();
        int ey = line.p2().y();
        if(sx > ex) {
          xsize = sx - ex + 1;
          xinc = -1;
        }
        else {
          xsize = ex - sx + 1;
          xinc = 1;
        }

        if(sy > ey) {
          ysize = sy - ey + 1;
          yinc = -1;
        }
        else {
          ysize = ey - sy + 1;
          yinc = 1;
        }

        if(ysize > xsize) {
          slope = (double)(ex - sx) / (double)(ey - sy);
          y = sy;
          for(i = 0; i < ysize; i++) {
            x = (int)(slope * (double)(y - sy) + (double) sx + 0.5);

            QPoint *pt = new QPoint;
            pt->setX(x);
            pt->setY(y);
            linePts->push_back(pt);
            y += yinc;
          }
        }
        else if(xsize == 1) {
          QPoint *pt = new QPoint;
          pt->setX(sx);
          pt->setY(sy);
          linePts->push_back(pt);
        }
        else {
          slope = (double)(ey - sy) / (double)(ex - sx);
          x = sx;
          for(i = 0; i < xsize; i++) {
            y = (int)(slope * (double)(x - sx) + (double) sy + 0.5);

            QPoint *pt = new QPoint;
            pt->setX(x);
            pt->setY(y);
            linePts->push_back(pt);
            x += xinc;
          }
        }

        if(linePts->empty()) {
          QMessageBox::information((QWidget *)parent(),
                                  "Error", "No points in edit line");
          return;
        }

        Brick *brick = new Brick(*cube, 1, 1, 1);

        //For each point read that value from the cube and add it to the histogram
        for(int i = 0; linePts && i < (int)linePts->size(); i++) {
          QPoint *pt = (*linePts)[i];
          int is = pt->x();
          int il = pt->y();
          brick->SetBasePosition(is, il, band);
          cube->read(*brick);
          hist.AddData(brick->DoubleBuffer(), 1);
        }
        delete brick;

        delete linePts;

      }
      //If rubber band is a rectangle
      else if(rubberBandTool()->currentMode() == RubberBandTool::RectangleMode) {
        double ssamp, sline, esamp, eline;

        // Convert vertices to line sample values
        activeViewport->viewportToCube(vertices[0].x(), vertices[0].y(),
                                       ssamp, sline);

        activeViewport->viewportToCube(vertices[2].x(), vertices[2].y(),
                                       esamp, eline);

        ssamp = round(ssamp);
        sline = round(sline);
        esamp = round(esamp);
        eline = round(eline);

        int nsamps = (int)(std::fabs(esamp - ssamp) + 1);

        Brick *brick = new Brick(*cube, nsamps, 1, 1);

        //For each line read nsamps and add it to the histogram
        for(int line = (int)std::min(sline, eline); line <= (int)std::max(sline, eline); line++) {
          int isamp = std::min(ssamp,esamp);
          brick->SetBasePosition(isamp, line, band);
          cube->read(*brick);
          hist.AddData(brick->DoubleBuffer(), nsamps);
        }
        delete brick;
      }
      //If rubber band is a polygon or circle
      else {
        geos::geom::Geometry *polygon = rubberBandTool()->geometry();

        std::vector <int> x_contained, y_contained;
        if(polygon != NULL) {
          const geos::geom::Envelope *envelope = polygon->getEnvelopeInternal();
          double ssamp, esamp, sline, eline;
          activeViewport->viewportToCube((int)floor(envelope->getMinX()),
                                         (int)floor(envelope->getMinY()),
                                         ssamp, sline);
          activeViewport->viewportToCube((int)ceil(envelope->getMaxX()),
                                         (int)ceil(envelope->getMaxY()),
                                         esamp, eline);


          for(int y = (int)sline; y <= (int)eline; y++) {
            for(int x = (int)ssamp; x <= (int)esamp; x++) {
              int x1, y1;
              activeViewport->cubeToViewport(x, y, x1, y1);
              geos::geom::Coordinate c(x1, y1);
              geos::geom::Point *p = globalFactory->createPoint(c);
              bool contains = p->within(polygon);
              delete p;

              if(contains) {
                x_contained.push_back(x);
                y_contained.push_back(y);
              }
            }
          }

          delete polygon;

          Brick *brick = new Brick(*cube, 1, 1, 1);

          //Read each point from the cube and add it to the histogram
          for(unsigned int j = 0; j < x_contained.size(); j++) {
            brick->SetBasePosition(x_contained[j], y_contained[j], band);
            cube->read(*brick);
            hist.AddData(brick->DoubleBuffer(), 1);
          }
          delete brick;
        }
      }


      //Transfer data from histogram to the plotcurve
      QVector<QPointF> binCountData;
      QVector<QPointF> cumPctData;
      double cumpct = 0.0;
      for(int i = 0; i < hist.Bins(); i++) {
        if(hist.BinCount(i) > 0) {
          binCountData.append(QPointF(hist.BinMiddle(i), hist.BinCount(i)));

          double pct = (double)hist.BinCount(i) / hist.ValidPixels() * 100.;
          cumpct += pct;
          cumPctData.append(QPointF(hist.BinMiddle(i), cumpct));
        }
      }

      //p_histCurve->setData(&xarray[0],&yarray[0],xarray.size());


      //These are all variables needed in the following for loop.
      //----------------------------------------------
      QVector<QwtIntervalSample> intervals(binCountData.size());
      double maxYValue = DBL_MIN;
      double minYValue = DBL_MAX;
      // ---------------------------------------------

      for(int y = 0; y < binCountData.size(); y++) {
        intervals[y].interval = QwtInterval(binCountData[y].x(), binCountData[y].x() + hist.BinSize());

        intervals[y].value = binCountData[y].y();
        if(binCountData[y].y() > maxYValue) maxYValue = binCountData[y].y();
        if(binCountData[y].y() < minYValue) minYValue = binCountData[y].y();
      }

      if (binCountData.size()) {
        validatePlotCurves();
        m_frequencyItem->setData(QwtIntervalSeriesData(intervals));
//         m_frequencyItem->setSource(activeViewport, vertices);
        m_percentageCurve->setData(new QwtPointSeriesData(cumPctData));
        m_percentageCurve->setSource(activeViewport, vertices);
      }


      QLabel *label = new QLabel("  Average = " + QString::number(hist.Average()) + '\n' +
                                "\n  Minimum = " + QString::number(hist.Minimum()) + '\n' +
                                "\n  Maximum = " + QString::number(hist.Maximum()) + '\n' +
                                "\n  Stand. Dev.= " + QString::number(hist.StandardDeviation()) + '\n' +
                                "\n  Variance = " + QString::number(hist.Variance()) + '\n' +
                                "\n  Median = " + QString::number(hist.Median()) + '\n' +
                                "\n  Mode = " + QString::number(hist.Mode()) + '\n' +
                                "\n  Skew = " + QString::number(hist.Skew()), targetWindow);


      QVBoxLayout *dockLayout = new QVBoxLayout;
      dockLayout->addWidget(label);
      dockLayout->addStretch();

      QWidget *dockContents = new QWidget;
      dockContents->setLayout(dockLayout);
      targetWindow->getDockWidget()->setWidget(dockContents);
      targetWindow->replot();
    }
  }


  /**
   * This method sets up the names, line style, and color  of the
   * all the plot items that will be used in this class. This
   * method also fills the p_colors QList with the colors that
   * will be used when the user copies and pastes (special) into
   * another plot window.
   */
  void HistogramTool::validatePlotCurves() {
      HistogramPlotWindow *targetWindow = qobject_cast<HistogramPlotWindow *>(
          selectedWindow());

    if (targetWindow) {
      if (!m_frequencyItem) {
        m_frequencyItem = new HistogramItem;
        m_frequencyItem->setYAxis(QwtPlot::yRight);
        m_frequencyItem->setColor(Qt::darkCyan);
        m_frequencyItem->setTitle("Frequency");
        targetWindow->add(m_frequencyItem);
      }

      QPen percentagePen(Qt::red);
      percentagePen.setWidth(2);

      if (!m_percentageCurve) {
        m_percentageCurve = createCurve("Percentage", percentagePen,
            CubePlotCurve::CubeDN, CubePlotCurve::Percentage);
        m_percentageCurve->setMarkerSymbol(QwtSymbol::NoSymbol);
        targetWindow->add(m_percentageCurve);
      }
    }
  }
}
