#include "HistogramTool.h"

#include <qwt_interval_data.h>

#include "Brick.h"
#include "HistogramToolWindow.h"
#include "MdiCubeViewport.h"
#include "PolygonTools.h"
#include "RubberBandComboBox.h"
#include "ToolPad.h"


namespace Qisis {

  /**
   * Constructor creates a new HistogramTool object.
   *
   * @param parent
   */
  HistogramTool::HistogramTool(QWidget *parent) : Qisis::PlotTool(parent) {
    p_rubberBand = NULL;
    RubberBandTool::allowPoints(1);
    p_parent = parent;
    createWindow();
    setupPlotCurves();
    p_scaled = false;
    p_action = new QAction(p_histToolWindow);
    p_action->setText("HistogramTool");
    p_action->setIcon(QPixmap(toolIconDir() + "/histogram.png"));
    QObject::connect(p_action, SIGNAL(activated()), this, SLOT(showPlotWindow()));
    QObject::connect(this, SIGNAL(viewportChanged()), this, SLOT(viewportSelected()));
    p_color = 0;
  }



  /**
   * This method is called when the tool is activated by the
   *   parent, or when the plot mode is changed. It's used to
   *   activate or change the rubber banding mode to be either
   *   rectangle or line, depending on the current plot type.
   */
  void HistogramTool::enableRubberBandTool() {
    if(p_rubberBand) {
      p_rubberBand->reset();
      p_rubberBand->setEnabled(true);
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
    QObject::connect(action, SIGNAL(activated()), this, SLOT(showPlotWindow()));

    p_histToolWindow->addViewMenu();

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
    QWidget *hbox = new QWidget(parent);

    p_rubberBand = new RubberBandComboBox(
      RubberBandComboBox::Rectangle |
      RubberBandComboBox::Line,
      RubberBandComboBox::Rectangle
    );

    QToolButton *newWindowButton = new QToolButton();
    newWindowButton->setText("New");
    newWindowButton->setToolTip("Opens a new blank plot window");
    QString windowText =
      "<b>Function:</b> This button will bring up a blank plot window that allows \
     the user to copy and paste curves from the active plot window to other windows \
    <p><b>Shortcut:</b>  CTRL+W</p>";
    newWindowButton->setWhatsThis(windowText);
    newWindowButton->setShortcut(Qt::CTRL + Qt::Key_W);
    connect(newWindowButton, SIGNAL(clicked()), this, SLOT(newPlotWindow()));

    QToolButton *plotButton = new QToolButton();
    plotButton->setText("Show");
    plotButton->setToolTip("Shows the active the plot window");
    QString text =
      "<b>Function:</b> This button will bring up the plot window that allows \
     the user to view the min, max, and average values of each band in a  \
     selected range of the image. <p><b>Shortcut:</b>  CTRL+L</p>";
    plotButton->setWhatsThis(text);
    plotButton->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(plotButton, SIGNAL(clicked()), this, SLOT(showPlotWindow()));

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(p_rubberBand);
    layout->addWidget(newWindowButton);
    layout->addWidget(plotButton);
    layout->addStretch(1);
    hbox->setLayout(layout);
    return hbox;
  }




  /**
   * This method updates the histogram tool.
   *
   */
  void HistogramTool::updateTool() {
    //If there is no viewport, disable the action
    if(cubeViewport() == NULL) {
      p_action->setEnabled(false);
    }
    //Else enable it and set the window's viewport to
    //the current viewport
    else {
      p_action->setEnabled(true);

      MdiCubeViewport *cvp = cubeViewport();
      p_histToolWindow->setViewport(cvp);
    }
  }


  /**
  * displays the plot window
  *
  */
  void HistogramTool::showPlotWindow() {
    p_histToolWindow->showWindow();
  }


  /**
   * This method creates the default histogram plot window.
   *
   */
  void HistogramTool::createWindow() {
    p_histToolWindow = new HistogramToolWindow("Active Histogram Window", p_parent);
    p_histToolWindow->setDestroyOnClose(false);
    p_histToolWindow->setDeletable(false);
    p_histToolWindow->setPasteable(false);
    //connect(p_histToolWindow, SIGNAL(curveCopied(Qisis::HistogramItem *)), this,
    //SLOT(copyCurve(Qisis::HistogramItem *)));

    QList<QMenu *> menu;
    QList<QAction *> actionButtons;

    /* menu is the QMenu's at the top of the plot window, while
     actionButtons are the buttons directly below.*/
    p_histToolWindow->getDefaultMenus(menu, actionButtons);

    for(int i = 0; i < menu.size(); i++) {
      if(menu[i]->title() == "&Options") {
        p_autoScale = new QAction(p_histToolWindow);
        p_autoScale->setText("AutoScale");
        p_autoScale->setCheckable(true);
        p_autoScale->setChecked(true);
        QString text  =
          "<b>Function:</b>  Turn on/off the auto scale option on the plot.";
        p_autoScale->setWhatsThis(text);
        menu[i]->addAction(p_autoScale);
        actionButtons.push_back(p_autoScale);
      }
    }

    p_histToolWindow->setCustomMenu(menu, actionButtons);
    p_histToolWindow->clearPlotCurves(false);
    p_histToolWindow->hideAllSymbols();
    p_histToolWindow->hideAllCurves();
  }


  /**
   * Called when the user has finished drawing with the rubber
   * band.  ChangePlot is called to plot the data within the
   * rubber band.
   *
   */
  void HistogramTool::rubberBandComplete() {
    p_histToolWindow->raise();
    if(RubberBandTool::isValid()) {
      changePlot();
    }
    else {
      QMessageBox::information(p_histToolWindow, "Error",
                               "The selected Area contains no valid pixels",
                               QMessageBox::Ok);
    }
  }


  /**
   * This method creates and displays a blank plot window in which
   * users can paste curves to and copy curves from.
   */
  void HistogramTool::newPlotWindow() {
    HistogramToolWindow *blankWindow = new HistogramToolWindow("Histogram Window", p_parent);
    blankWindow->setDestroyOnClose(true);
    connect(blankWindow, SIGNAL(curvePaste(Qisis::PlotWindow *)), this,
            SLOT(pasteCurve(Qisis::PlotWindow *)));
    connect(blankWindow, SIGNAL(curvePasteSpecial(Qisis::PlotWindow *)), this,
            SLOT(pasteCurveSpecial(Qisis::PlotWindow *)));
    connect(blankWindow, SIGNAL(curveCopied(Qisis::PlotCurve *)), this,
            SLOT(copyCurve(Qisis::PlotCurve *)));
    connect(blankWindow, SIGNAL(destroyed(QObject *)), this,
            SLOT(removeWindow(QObject *)));
    connect(blankWindow, SIGNAL(plotChanged()), this, SLOT(updateViewPort()));
    blankWindow->setScale(QwtPlot::xBottom, p_histToolWindow->p_xMin,
                          p_histToolWindow->p_xMax);
    blankWindow->setScale(QwtPlot::yLeft, p_histToolWindow->p_yMin,
                          p_histToolWindow->p_yMax);
    blankWindow->setPlotTitle(p_histToolWindow->getPlotTitle().text());
    blankWindow->setDeletable(true);
    blankWindow->setPasteable(true);
    blankWindow->setCopyEnable(false);
    blankWindow->setupDefaultMenu();
    blankWindow->hideAllSymbols();
    blankWindow->hideAllCurves();
    blankWindow->showWindow();

    p_plotWindows.push_back(blankWindow);
  }


  /**
   * This method plots the selected data in a histogram window.
   */
  void HistogramTool::changePlot() {
    MdiCubeViewport *cvp = cubeViewport();
    /* Delete any current curves*/
    p_histToolWindow->clearPlotCurves();

    QList<QPoint> vertices;

    if(RubberBandTool::getMode() == RubberBandTool::Circle) {
      geos::geom::Geometry *p = RubberBandTool::geometry();
      geos::geom::CoordinateSequence *c = p->getCoordinates();
      for(int i = 0; i < (int)c->getSize(); i++) {
        QPoint point((int)(c->getX(i) + 0.5), (int)(c->getY(i) + 0.5));
        vertices.append(point);
      }
      delete p;
    }
    else {
      vertices = RubberBandTool::getVertices();
    }

    p_histCurve->setViewPort(cvp);
    p_histCurve->setVertices(vertices);

    p_cdfCurve->setViewPort(cvp);
    p_cdfCurve->setVertices(vertices);

    if(vertices.size() < 1) return;

    Isis::Cube *cube = cvp->cube();
    int band = cvp->grayBand();
    Isis::Histogram hist(*cube, 1);

    //If the rubber band is a line
    if(RubberBandTool::getMode() == RubberBandTool::Line) {
      double ssamp, sline, esamp, eline;
      cvp->viewportToCube(vertices[0].rx(), vertices[0].ry(),
                          ssamp, sline);

      cvp->viewportToCube(vertices[1].rx(), vertices[1].ry(),
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

      Isis::Brick *brick = new Isis::Brick(*cube, 1, 1, 1);

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
    else if(RubberBandTool::getMode() == RubberBandTool::Rectangle) {
      double ssamp, sline, esamp, eline;

      // Convert them to line sample values
      cvp->viewportToCube(vertices[0].x(), vertices[0].y(), ssamp, sline);
      cvp->viewportToCube(vertices[2].x(), vertices[2].y(), esamp, eline);

      ssamp = ssamp + 0.5;
      sline = sline + 0.5;
      esamp = esamp + 0.5;
      eline = eline + 0.5;

      int nsamps = (int)(esamp - ssamp + 1);
      if(nsamps < 1) nsamps = -nsamps;

      Isis::Brick *brick = new Isis::Brick(*cube, nsamps, 1, 1);

      //For each line read nsamps and add it to the histogram
      for(int line = (int)std::min(sline, eline); line <= (int)std::max(sline, eline); line++) {
        brick->SetBasePosition((int)ssamp, line, band);
        cube->read(*brick);
        hist.AddData(brick->DoubleBuffer(), nsamps);
      }
      delete brick;
    }
    //If rubber band is a polygon or circle
    else {
      geos::geom::Geometry *polygon = RubberBandTool::geometry();

      std::vector <int> x_contained, y_contained;
      if(polygon != NULL) {
        const geos::geom::Envelope *envelope = polygon->getEnvelopeInternal();
        double ssamp, esamp, sline, eline;
        cvp->viewportToCube((int)floor(envelope->getMinX()), (int)floor(envelope->getMinY()), ssamp, sline);
        cvp->viewportToCube((int)ceil(envelope->getMaxX()), (int)ceil(envelope->getMaxY()), esamp, eline);


        for(int y = (int)sline; y <= (int)eline; y++) {
          for(int x = (int)ssamp; x <= (int)esamp; x++) {
            int x1, y1;
            cvp->cubeToViewport(x, y, x1, y1);
            geos::geom::Coordinate c(x1, y1);
            geos::geom::Point *p = Isis::globalFactory.createPoint(c);
            bool contains = p->within(polygon);
            delete p;

            if(contains) {
              x_contained.push_back(x);
              y_contained.push_back(y);
            }
          }
        }

        delete polygon;

        Isis::Brick *brick = new Isis::Brick(*cube, 1, 1, 1);

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
    std::vector<double> xarray, yarray, y2array;
    double cumpct = 0.0;
    for(int i = 0; i < hist.Bins(); i++) {
      if(hist.BinCount(i) > 0) {
        xarray.push_back(hist.BinMiddle(i));
        yarray.push_back(hist.BinCount(i));

        double pct = (double)hist.BinCount(i) / hist.ValidPixels() * 100.;
        cumpct += pct;
        y2array.push_back(cumpct);
      }
    }

    //p_histCurve->setData(&xarray[0],&yarray[0],xarray.size());


    //These are all variables needed in the following for loop.
    //----------------------------------------------
    QwtArray<QwtDoubleInterval> intervals(xarray.size());
    QwtValueList majorTicks;
    QwtArray<double> values(yarray.size());
    double maxYValue = DBL_MIN;
    double minYValue = DBL_MAX;
    // ---------------------------------------------

    for(unsigned int y = 0; y < yarray.size(); y++) {

      intervals[y] = QwtDoubleInterval(xarray[y], xarray[y] + hist.BinSize());

      majorTicks.push_back(xarray[y]);
      //std::cout << "\nmajor tick " << xarray[y] << std::endl;
      majorTicks.push_back(xarray[y] + hist.BinSize());
      //std::cout << "& " << xarray[y] + hist.BinSize() << std::endl;

      values[y] = yarray[y];
      if(values[y] > maxYValue) maxYValue = values[y];
      if(values[y] < minYValue) minYValue = values[y];
    }

    QwtScaleDiv scaleDiv;
    scaleDiv.setTicks(QwtScaleDiv::MajorTick, majorTicks);

    p_histCurve->setData(QwtIntervalData(intervals, values));
    p_cdfCurve->setData(&xarray[0], &y2array[0], xarray.size());
    p_cdfCurve->setVisible(true);
    p_cdfCurve->setSymbolVisible(false);

    p_histToolWindow->add(p_histCurve);
    p_histToolWindow->add(p_cdfCurve);
    p_histToolWindow->fillTable();

    if(p_autoScale->isChecked()) {
      p_histToolWindow->setScale(QwtPlot::yLeft, 0, maxYValue);
      p_histToolWindow->setScale(QwtPlot::xBottom, hist.Minimum(), hist.Maximum());
      //p_histToolWindow->setScale(QwtPlot::xBottom,0,hist.Maximum());
      //std::cout << "hist.Minimum() = " << hist.Minimum() << " xarray[0] = " << xarray[0] << std::endl;
      //This line causes a seg. fault if the xarray has no data!
      //p_histToolWindow->setScale(QwtPlot::xBottom,xarray[0],hist.Maximum());
      //p_histToolWindow->setScaleDiv(QwtPlot::xBottom, scaleDiv);
    }

    copyCurve();
    p_histToolWindow->p_curveCopied = true;
    p_histToolWindow->showWindow();
    updateTool();


    QLabel *label = new QLabel("  Average = " + QString::number(hist.Average()) + '\n' +
                               "\n  Minimum = " + QString::number(hist.Minimum()) + '\n' +
                               "\n  Maximum = " + QString::number(hist.Maximum()) + '\n' +
                               "\n  Stand. Dev.= " + QString::number(hist.StandardDeviation()) + '\n' +
                               "\n  Variance = " + QString::number(hist.Variance()) + '\n' +
                               "\n  Median = " + QString::number(hist.Median()) + '\n' +
                               "\n  Mode = " + QString::number(hist.Mode()) + '\n' +
                               "\n  Skew = " + QString::number(hist.Skew()), p_histToolWindow);
    p_histToolWindow->getDockWidget()->setWidget(label);
  }


  /**
   * This method creates a new HistogramItem and copies the
   * properties of the curve the user clicked on into the new
   * curve. The plotWindow class emits a signal when a curve has
   * been requested to be copied.
   *
   * @param pc
   */
  void HistogramTool::copyCurve(Qisis::PlotCurve *pc) {
    p_copyCurve = new HistogramItem();
    p_copyCurve->copyCurveProperties(p_histCurve);
  }

  void HistogramTool::copyCurve() {
    p_copyCurve = new HistogramItem();
    p_copyCurve->copyCurveProperties(p_histCurve);
  }


  /**
   * This method pastes the copied curve into the given plot
   * window.  The plotWindow class emits a signal when a paste
   * command has taken place inside the window.
   * @param pw
   */
  void HistogramTool::pasteCurve(Qisis::PlotWindow *pw) {
    p_cvp = cubeViewport();
    ((HistogramToolWindow *)pw)->add(p_copyCurve);
    updateViewPort(p_cvp);

  }


  /**
   * This method does the same as the above method but gives the
   * curve a different color than the copied curve.
   * @param pw
   */
  void HistogramTool::pasteCurveSpecial(Qisis::PlotWindow *pw) {
    p_cvp = cubeViewport();
    if(p_color < p_colors.size()) {
      p_copyCurve->setColor(p_colors[p_color]);
    }
    else {
      QColor c = QColorDialog::getColor(Qt::white, p_histToolWindow);
      if(c.isValid()) {
        p_copyCurve->setColor(c);
      }
    }

    ((HistogramToolWindow *)pw)->add(p_copyCurve);
    updateViewPort(p_cvp);
    p_color++;

  }



  /**
   * This method sets up the names, line style, and color  of the
   * all the plot items that will be used in this class. This
   * method also fills the p_colors QList with the colors that
   * will be used when the user copies and pastes (special) into
   * another plot window.
   */
  void HistogramTool::setupPlotCurves() {
    p_histCurve = new HistogramItem();
    p_histCurve->setColor(Qt::darkCyan);
    //If we give the curve a title, it will show up on the legend.
    //p_histCurve->setTitle("Frequency");

    p_cdfCurve = new PlotToolCurve();
    p_cdfCurve->setStyle(QwtPlotCurve::Lines);
    p_cdfCurve->setTitle("Percentage");

    QPen *pen = new QPen(Qt::darkCyan);
    pen->setWidth(2);
    p_histCurve->setYAxis(QwtPlot::yLeft);
    //p_histCurve->setPen(*pen);
    pen->setColor(Qt::red);

    p_cdfCurve->setYAxis(QwtPlot::yRight);
    p_cdfCurve->setPen(*pen);

    p_colors.push_back(Qt::cyan);
    p_colors.push_back(Qt::magenta);
    p_colors.push_back(Qt::yellow);
    p_colors.push_back(QColor(255, 170, 255));
    p_colors.push_back(Qt::green);
    p_colors.push_back(Qt::white);
    p_colors.push_back(Qt::blue);
    p_colors.push_back(Qt::red);
    p_colors.push_back(QColor(134, 66, 176));
    p_colors.push_back(QColor(255, 152, 0));

  }


  /**
   * This method paints the polygons of the copied hist. items
   * onto the cubeviewport
   *
   * @param vp
   * @param painter
   */
  void HistogramTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {

    int sample1, line1, sample2, line2;

    for(int i = 0; i < p_plotWindows.size(); i++) {
      for(int c = 0; c < p_plotWindows[i]->getNumItems(); c++) {
        HistogramItem *histItem = p_plotWindows[i]->getHistItem(c);

        if(histItem->getViewPort() == vp) {
          QPen pen(histItem->color());
          pen.setWidth(2);
          pen.setStyle(Qt::SolidLine);
          painter->setPen(pen);
          QList <QPointF> points = histItem->getVertices();

          for(int p = 1; p < points.size(); p++) {
            vp->cubeToViewport(points[p-1].x(), points[p-1].y(), sample1, line1);
            vp->cubeToViewport(points[p].x(), points[p].y(), sample2, line2);
            painter->drawLine(QPoint(sample1, line1), QPoint(sample2,  line2));
          }

          vp->cubeToViewport(points[points.size()-1].x(),
                             points[points.size()-1].y(), sample1, line1);
          vp->cubeToViewport(points[0].x(), points[0].y(), sample2, line2);
          painter->drawLine(QPoint(sample1, line1), QPoint(sample2,  line2));
        }

      }

    }
  }


  /**
   * This method causes the view port corresponding with the given PlotToolCurve
   * to be repainted with all of the area's of interest associated with the
   * PlotToolCurve's plotwindow. The paintViewport() method is called.
   *
   * @param cvp
   */
  void HistogramTool::updateViewPort(MdiCubeViewport *cvp) {
    cvp->repaint();
  }

}

