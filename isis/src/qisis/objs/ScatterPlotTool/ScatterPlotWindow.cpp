#include "IsisDebug.h"
#include "ScatterPlotWindow.h"

#include <QVector>

#include <qwt_color_map.h>
#include <qwt_double_range.h>
#include <qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>

#include "Histogram.h"
#include "MdiCubeViewport.h"
#include "Portal.h"
#include "ScatterPlotAlarmConfigDialog.h"
#include "ScatterPlotData.h"
#include "ScatterPlotTool.h"
#include "ViewportBuffer.h"
#include "ViewportMainWindow.h"
#include "Workspace.h"

using namespace std;

namespace Isis {

  ScatterPlotWindow::ScatterPlotWindow(QString title,
      Cube *xAxisCube, int xAxisBand, int xAxisBinCount,
      Cube *yAxisCube, int yAxisBand, int yAxisBinCount,
      QwtDoubleRange sampleRange, QwtDoubleRange lineRange,
      QWidget *parent) :
      PlotWindow("Scatter Plot", PlotCurve::CubeDN, PlotCurve::CubeDN, parent,
                 (MenuOptions)(
                   AllMenuOptions &
                     ~BackgroundSwitchMenuOption &
                     ~ShowTableMenuOption &
                     ~ClearPlotMenuOption &
                     ~ShowHideMarkersMenuOption &
                     ~ShowHideCurvesMenuOption)) {
    m_xAxisCube = xAxisCube;
    m_yAxisCube = yAxisCube;
    m_xAxisCubeBand = xAxisBand;
    m_yAxisCubeBand = yAxisBand;
    m_sampleRange = sampleRange;
    m_lineRange = lineRange;
    m_xCubeDnAlarmRange.first = Null;
    m_xCubeDnAlarmRange.second = Null;
    m_yCubeDnAlarmRange.first = Null;
    m_yCubeDnAlarmRange.second = Null;

    ScatterPlotData *data = new ScatterPlotData(
        xAxisCube, xAxisBand, xAxisBinCount,
        yAxisCube, yAxisBand, yAxisBinCount,
        sampleRange, lineRange);

    m_spectrogram = new QwtPlotSpectrogram;

    m_spectrogram->setData(*data);
    m_spectrogram->setTitle("Scatter Plot Counts");
    m_spectrogram->attach(plot());

    disableAxisAutoScale();
    zoomer()->zoom(0);
    plot()->setAxisScale(QwtPlot::xBottom, data->xCubeMin(), data->xCubeMax());
    plot()->setAxisScale(QwtPlot::yLeft, data->yCubeMin(), data->yCubeMax());
    zoomer()->setZoomBase();
    replot();

    QwtScaleWidget *rightAxis = plot()->axisWidget(QwtPlot::yRight);
    rightAxis->setTitle("Counts");
    rightAxis->setColorBarEnabled(true);
    rightAxis->setColorMap(m_spectrogram->data().range(),
                             m_spectrogram->colorMap());

    plot()->setAxisScale(QwtPlot::yRight,
                         m_spectrogram->data().range().minValue(),
                         m_spectrogram->data().range().maxValue());
    plot()->enableAxis(QwtPlot::yRight);

    plot()->setAxisTitle(QwtPlot::xBottom,
        QFileInfo(xAxisCube->getFilename()).baseName() + " Band " +
        QString::number(xAxisBand) + " " +
        plot()->axisTitle(QwtPlot::xBottom).text());
    plot()->setAxisTitle(QwtPlot::yLeft,
        QFileInfo(yAxisCube->getFilename()).baseName() + " Band " +
        QString::number(yAxisBand) + " " +
        plot()->axisTitle(QwtPlot::yLeft).text());

    QwtValueList contourLevels;
    QwtDoubleInterval range = data->range();

    for (double level = range.minValue();
         level < range.maxValue();
         level += ((range.maxValue() - range.minValue()) / 6.0)) {
      contourLevels += level;
    }

    m_spectrogram->setContourLevels(contourLevels);

    m_colorize = new QAction(this);
    m_colorize->setText("Colorize");
    m_colorize->setIcon(QPixmap(Filename("$base/icons/rgb.png").Expanded()));
    connect(m_colorize, SIGNAL(activated()),
            this, SLOT(colorPlot()));

    m_contour = new QAction(this);
    m_contour->setText("Hide Contour Lines");
    m_contour->setIcon(
        QPixmap(Filename("$base/icons/scatterplotcontour.png").Expanded()));
    connect(m_contour, SIGNAL(activated()),
            this, SLOT(showHideContour()));

    QAction *configureAlarmingAct = new QAction(this);
    configureAlarmingAct->setText("Change Alarming");
    configureAlarmingAct->setIcon(
        QPixmap(Filename("$base/icons/scatterplotalarming.png").Expanded()));
    connect(configureAlarmingAct, SIGNAL(activated()),
            this, SLOT(configureAlarming()));

    foreach (QAction *menuAction, menuBar()->actions()) {
      if (menuAction->text() == "&Options") {
        QMenu *optsMenu = qobject_cast<QMenu *>(menuAction->parentWidget());
        optsMenu->addAction(m_colorize);
        optsMenu->addAction(m_contour);
        optsMenu->addAction(configureAlarmingAct);
      }
    }

    colorPlot();
    showHideContour();

    plot()->canvas()->installEventFilter(this);
    plot()->canvas()->setMouseTracking(true);

    replot();

    std::string instanceName = windowTitle().toStdString();
    Filename config("$HOME/.Isis/qview/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()),
                       QSettings::NativeFormat);
    m_alarmPlot = settings.value("alarmOntoPlot", true).toBool();
    m_alarmViewport = settings.value("alarmOntoViewport", true).toBool();

    m_alarmPlotSamples = settings.value("alarmPlotSamples", 25).toInt();
    m_alarmPlotLines = settings.value("alarmPlotLines", 25).toInt();

    m_alarmViewportUnits = (AlarmRangeUnits)settings.value("alarmViewportUnits",
                                                           ScreenUnits).toInt();

    m_alarmViewportScreenWidth =
        settings.value("alarmViewportScreenWidth", 5).toInt();
    m_alarmViewportScreenHeight =
        settings.value("alarmViewportScreenHeight", 5).toInt();

    m_alarmViewportXDnBoxSize =
        settings.value("alarmViewportXDnBoxSize", 1.0).toDouble();
    m_alarmViewportYDnBoxSize =
        settings.value("alarmViewportYDnBoxSize", 1.0).toDouble();
  }


  ScatterPlotWindow::~ScatterPlotWindow() {
    std::string instanceName = windowTitle().toStdString();
    Filename config("$HOME/.Isis/qview/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()),
                       QSettings::NativeFormat);
    settings.setValue("alarmOntoPlot", m_alarmPlot);
    settings.setValue("alarmOntoViewport", m_alarmViewport);

    settings.setValue("alarmPlotSamples", m_alarmPlotSamples);
    settings.setValue("alarmPlotLines", m_alarmPlotLines);

    settings.setValue("alarmViewportUnits", (int)m_alarmViewportUnits);

    settings.setValue("alarmViewportScreenWidth", m_alarmViewportScreenWidth);
    settings.setValue("alarmViewportScreenHeight", m_alarmViewportScreenHeight);

    settings.setValue("alarmViewportXDnBoxSize", m_alarmViewportXDnBoxSize);
    settings.setValue("alarmViewportYDnBoxSize", m_alarmViewportYDnBoxSize);
  }


  bool ScatterPlotWindow::alarmingPlot() const {
    return m_alarmPlot;
  }


  bool ScatterPlotWindow::alarmingViewport() const {
    return m_alarmViewport;
  }


  QPair<int, int> ScatterPlotWindow::alarmPlotBoxSize() const {
    return QPair<int, int>(m_alarmPlotSamples, m_alarmPlotLines);
  }


  ScatterPlotWindow::AlarmRangeUnits ScatterPlotWindow::alarmViewportUnits() const {
    return m_alarmViewportUnits;
  }


  QPair<int, int> ScatterPlotWindow::alarmViewportScreenBoxSize() const {
    return QPair<int, int>(m_alarmViewportScreenWidth,
                           m_alarmViewportScreenHeight);
  }


  QPair<double, double> ScatterPlotWindow::alarmViewportDnBoxSize() const {
    return QPair<double, double>(m_alarmViewportXDnBoxSize,
                                 m_alarmViewportYDnBoxSize);
  }


  bool ScatterPlotWindow::eventFilter(QObject *o, QEvent *e) {
    if (o == plot()->canvas()) {
      switch (e->type()) {
        case QEvent::MouseMove: {
          if (((QMouseEvent *)e)->buttons() == Qt::NoButton)
            mouseMoveEvent((QMouseEvent *)e);
          break;
        }
        case QEvent::Leave: {
          mouseLeaveEvent((QMouseEvent *)e);
          break;
        }
        default:
          break;
      }

      return false;
    }
    else {
      return PlotWindow::eventFilter(o, e);
    }
  }


  void ScatterPlotWindow::paint(MdiCubeViewport *vp, QPainter *painter) {
    PlotWindow::paint(vp, painter);

    // Do alarming from plot onto viewport
    if (alarmingViewport() &&
        !IsSpecial(m_xCubeDnAlarmRange.first) &&
        !IsSpecial(m_xCubeDnAlarmRange.second) &&
        !IsSpecial(m_yCubeDnAlarmRange.first) &&
        !IsSpecial(m_yCubeDnAlarmRange.second)) {
      painter->setPen(QPen(Qt::red));

      ViewportBuffer *buffer = vp->grayBuffer();

      if ((isXCube(vp) || isYCube(vp)) && buffer && !buffer->working()) {
        int numLines = buffer->bufferXYRect().height();

        QScopedPointer<Portal> portal;

        // We are going to read DNs from the cube that isn't in the passed in
        //   viewport. For example, if we're painting X, we're missing the
        //   corresponding Y DN values.
        if (isXCube(vp))
          portal.reset(new Portal(1, 1, m_yAxisCube->getPixelType()));
        else
          portal.reset(new Portal(1, 1, m_xAxisCube->getPixelType()));

        // Iterate through the in-memory DN values for the passed in viewport
        for (int yIndex = 0; yIndex < numLines; yIndex++) {
          const vector<double> &line = buffer->getLine(yIndex);

          for (int xIndex = 0; xIndex < (int)line.size(); xIndex++) {
            int viewportPointX = xIndex + buffer->bufferXYRect().left();
            int viewportPointY = yIndex + buffer->bufferXYRect().top();

            double cubeSample = Null;
            double cubeLine = Null;
            vp->viewportToCube(viewportPointX, viewportPointY,
                               cubeSample, cubeLine);

            // The sample/line range is the actual scatter plotted sample/line
            //   range. Don't alarm outside of this range on the cube ever.
            if (cubeSample >= m_sampleRange.minValue() - 0.5 &&
                cubeSample <= m_sampleRange.maxValue() + 0.5 &&
                cubeLine >= m_lineRange.minValue() - 0.5 &&
                cubeLine <= m_lineRange.maxValue() + 0.5) {
              // If the in-memory DN values are within the alarm box range, check
              //   the corresponding DN values for the other axis via cube I/O.
              if (isXCube(vp) &&
                  line[xIndex] >= m_xCubeDnAlarmRange.first &&
                  line[xIndex] <= m_xCubeDnAlarmRange.second) {
                portal->SetPosition(cubeSample, cubeLine, m_yAxisCubeBand);
                m_yAxisCube->read(*portal);

                double yDnValue = (*portal)[0];

                if (yDnValue >= m_yCubeDnAlarmRange.first &&
                    yDnValue <= m_yCubeDnAlarmRange.second) {
                  painter->drawPoint(viewportPointX, viewportPointY);
                }
              }
              else if (isYCube(vp) &&
                       line[xIndex] >= m_yCubeDnAlarmRange.first &&
                       line[xIndex] <= m_yCubeDnAlarmRange.second) {
                portal->SetPosition(cubeSample, cubeLine, m_xAxisCubeBand);
                m_xAxisCube->read(*portal);

                double xDnValue = (*portal)[0];

                if (xDnValue >= m_xCubeDnAlarmRange.first &&
                    xDnValue <= m_xCubeDnAlarmRange.second) {
                  painter->drawPoint(viewportPointX, viewportPointY);
                }
              }
            }
          }
        }
      }
    }
  }


  /**
   * Saves the current mouse position in the viewport so that the plot can be
   * alarmed around that position.
   */
  void ScatterPlotWindow::setMousePosition(MdiCubeViewport *vp,
                                           QPoint mouseLoc) {
    const ScatterPlotData *scatterData =
        dynamic_cast<const ScatterPlotData *>(&m_spectrogram->data());

    if (scatterData) {
      scatterData->clearAlarms();

      if (alarmingPlot() && (isXCube(vp) || isYCube(vp))) {
        QScopedPointer<Portal> xCubePortal(
            new Portal(m_alarmPlotSamples, m_alarmPlotLines,
                       m_xAxisCube->getPixelType()));
        QScopedPointer<Portal> yCubePortal(
            new Portal(m_alarmPlotSamples, m_alarmPlotLines,
                       m_yAxisCube->getPixelType()));

        double cubeSample = Null;
        double cubeLine = Null;

        vp->viewportToCube(mouseLoc.x(), mouseLoc.y(), cubeSample, cubeLine);

        // The sample/line range is the actual scatter plotted sample/line
        //   range. Don't alarm outside of this range on the cube ever.
        if (cubeSample >= m_sampleRange.minValue() - 0.5 &&
            cubeSample <= m_sampleRange.maxValue() + 0.5 &&
            cubeLine >= m_lineRange.minValue() - 0.5 &&
            cubeLine <= m_lineRange.maxValue() + 0.5) {
          xCubePortal->SetPosition(cubeSample, cubeLine, m_xAxisCubeBand);
          m_xAxisCube->read(*xCubePortal);
          yCubePortal->SetPosition(cubeSample, cubeLine, m_yAxisCubeBand);
          m_yAxisCube->read(*yCubePortal);

          ASSERT(xCubePortal->size() == yCubePortal->size());
          for (int i = 0; i < xCubePortal->size(); i++) {
            double x = (*xCubePortal)[i];
            double y = (*yCubePortal)[i];

            if (!IsSpecial(x) && !IsSpecial(y)) {
              scatterData->alarm(x, y);
            }
          }
        }
      }

      plot()->replot();
    }
  }


  void ScatterPlotWindow::setAlarmingPlot(bool alarming) {
    m_alarmPlot = alarming;
  }


  void ScatterPlotWindow::setAlarmingViewport(bool alarming) {
    m_alarmViewport = alarming;
  }


  void ScatterPlotWindow::setAlarmPlotBoxSize(int samples, int lines) {
    m_alarmPlotSamples = samples;
    m_alarmPlotLines = lines;
  }


  void ScatterPlotWindow::setAlarmViewportUnits(AlarmRangeUnits units) {
    m_alarmViewportUnits = units;
  }


  void ScatterPlotWindow::setAlarmViewportScreenBoxSize(int width, int height) {
    m_alarmViewportScreenWidth = width;
    m_alarmViewportScreenHeight = height;
  }


  void ScatterPlotWindow::setAlarmViewportDnBoxSize(double xDnBoxSize,
                                                    double yDnBoxSize) {
    m_alarmViewportXDnBoxSize = xDnBoxSize;
    m_alarmViewportYDnBoxSize = yDnBoxSize;
  }


  void ScatterPlotWindow::forgetCubes() {
    m_xAxisCube = NULL;
    m_yAxisCube = NULL;
  }


  /**
   * This method switches the color mode of the scatter plot from
   * black and white to color and visa versa.
   *
   */
  void ScatterPlotWindow::colorPlot() {
    if (m_colorize->text().compare("Colorize") == 0) {
      m_colorize->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/gray.png"));
      m_colorize->setText("Gray");
      QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
      colorMap.addColorStop(DBL_EPSILON, Qt::cyan);
      colorMap.addColorStop(0.3, Qt::green);
      colorMap.addColorStop(0.50, Qt::yellow);
      m_spectrogram->setColorMap(colorMap);
      plot()->setCanvasBackground(Qt::darkCyan);
    }
    else {
      m_colorize->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/rgb.png"));
      m_colorize->setText("Colorize");
      QwtLinearColorMap colorMap(Qt::black, Qt::white);
      colorMap.addColorStop(DBL_EPSILON, Qt::darkGray);
      m_spectrogram->setColorMap(colorMap);
      plot()->setCanvasBackground(Qt::black);
    }

    plot()->axisWidget(QwtPlot::yRight)->setColorMap(
        m_spectrogram->data().range(),
        m_spectrogram->colorMap());
    updateContourPen();

    replot();
  }


  /**
   * This method hides or displays the contour lines on the
   * spectrogram.
   *
   */
  void ScatterPlotWindow::showHideContour() {
    if (m_contour->text() == "Show Contour Lines") {
      m_contour->setText("Hide Contour Lines");
      m_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
      updateContourPen();
    }
    else {
      m_contour->setText("Show Contour Lines");
      m_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, false);
    }

    replot();
  }


  /**
   * Returns true if the viewport's cube is the cube currently being used on the
   * x-axis.
   */
  bool ScatterPlotWindow::isXCube(MdiCubeViewport *vp) const {
    return (vp && m_xAxisCube &&
            vp->cube() == m_xAxisCube && vp->grayBand() == m_xAxisCubeBand &&
            vp->isGray());
  }


  /**
   * Returns true if the viewport's cube is the cube currently being used on the
   * y-axis.
   */
  bool ScatterPlotWindow::isYCube(MdiCubeViewport *vp) const {
    return (vp && m_yAxisCube &&
            vp->cube() == m_yAxisCube && vp->grayBand() == m_yAxisCubeBand &&
            vp->isGray());
  }


  void ScatterPlotWindow::mouseMoveEvent(QMouseEvent *e) {
    if (alarmingViewport()) {
      if (m_alarmViewportUnits == ScreenUnits) {
        m_xCubeDnAlarmRange.first = plot()->invTransform(
            QwtPlot::xBottom, e->pos().x() - m_alarmViewportScreenWidth / 2);
        m_xCubeDnAlarmRange.second = plot()->invTransform(
            QwtPlot::xBottom, e->pos().x() + m_alarmViewportScreenWidth / 2);

        m_yCubeDnAlarmRange.first = plot()->invTransform(
            QwtPlot::yLeft, e->pos().y() + m_alarmViewportScreenHeight / 2);
        m_yCubeDnAlarmRange.second = plot()->invTransform(
            QwtPlot::yLeft, e->pos().y() - m_alarmViewportScreenHeight / 2);

        if (m_xCubeDnAlarmRange.first > m_xCubeDnAlarmRange.second)
          std::swap(m_xCubeDnAlarmRange.first, m_xCubeDnAlarmRange.second);

        if (m_yCubeDnAlarmRange.first > m_yCubeDnAlarmRange.second)
          std::swap(m_yCubeDnAlarmRange.first, m_yCubeDnAlarmRange.second);
      }
      else {
        m_xCubeDnAlarmRange.first = plot()->invTransform(
            QwtPlot::xBottom, e->pos().x()) - m_alarmViewportXDnBoxSize / 2.0;
        m_xCubeDnAlarmRange.second = plot()->invTransform(
            QwtPlot::xBottom, e->pos().x()) + m_alarmViewportXDnBoxSize / 2.0;

        m_yCubeDnAlarmRange.first = plot()->invTransform(
            QwtPlot::yLeft, e->pos().y()) - m_alarmViewportYDnBoxSize / 2.0;
        m_yCubeDnAlarmRange.second = plot()->invTransform(
            QwtPlot::yLeft, e->pos().y()) + m_alarmViewportYDnBoxSize / 2.0;
      }
    }
    else {
      m_xCubeDnAlarmRange.first = Null;
      m_xCubeDnAlarmRange.second = Null;
      m_yCubeDnAlarmRange.first = Null;
      m_yCubeDnAlarmRange.second = Null;
    }

    emit plotChanged();
  }


  void ScatterPlotWindow::mouseLeaveEvent(QMouseEvent *e) {
    m_xCubeDnAlarmRange.first = Null;
    m_xCubeDnAlarmRange.second = Null;
    m_yCubeDnAlarmRange.first = Null;
    m_yCubeDnAlarmRange.second = Null;

    emit plotChanged();
  }


  void ScatterPlotWindow::updateContourPen() {
    if (m_colorize->text() == "Gray") {
      m_spectrogram->setDefaultContourPen(QPen(Qt::red));
    }
    else {
      m_spectrogram->setDefaultContourPen(QPen(Qt::white));
    }
  }


  void ScatterPlotWindow::configureAlarming() {
    ScatterPlotAlarmConfigDialog * config =
        new ScatterPlotAlarmConfigDialog(this);
    connect(config, SIGNAL(finished(int)),
            config, SLOT(deleteLater()));
    config->show();
  }
}

