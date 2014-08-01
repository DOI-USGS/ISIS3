#ifndef ScatterPlotWindow_h
#define ScatterPlotWindow_h

// Only Parents of classes defined in this file should be included in this file!
#include "PlotWindow.h"

#include <qwt_double_range.h>

#include <QList>
#include <QScopedPointer>

class QwtPlotSpectrogram;
class QwtPlotMarker;

namespace Isis {
  class Cube;
  class ScatterPlotTool;

  /**
   * @brief Scatter Plot Window
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2014-06-23 Ian Humphrey - Modified hard coded /usgs/cpkgs/ paths to 
   *                           relative pathnames. Fixes #2054.
   *   @history 2014-07-31 Ian Humphrey - Removed ConfigurePlotMenuOption for scatter plot window.
   *                           References #2089.
   */
  class ScatterPlotWindow : public PlotWindow {
      Q_OBJECT

    public:
      /**
       * This enumeration differentiates alarming a strict cube DN box
       *   size from a screen region.
       */
      enum AlarmRangeUnits {
        //! Alarming is a DN range around the mouse
        CubeUnits,
        //! Alarming is a visible area around the mouse
        ScreenUnits
      };

      ScatterPlotWindow(QString title,
                        Cube *xAxisCube, int xAxisBand, int xAxisBinCount,
                        Cube *yAxisCube, int yAxisBand, int yAxisBinCount,
                        QwtInterval sampleRange, QwtInterval lineRange,
                        QWidget *parent);
      virtual ~ScatterPlotWindow();

      bool alarmingPlot() const;
      bool alarmingViewport() const;
      QPair<int, int> alarmPlotBoxSize() const;
      AlarmRangeUnits alarmViewportUnits() const;
      QPair<int, int> alarmViewportScreenBoxSize() const;
      QPair<double, double> alarmViewportDnBoxSize() const;

      bool eventFilter(QObject *o, QEvent *e);

      virtual void paint(MdiCubeViewport *vp, QPainter *painter);

      void setMousePosition(MdiCubeViewport *vp, QPoint mouseLoc);

      void setAlarmingPlot(bool);
      void setAlarmingViewport(bool);
      void setAlarmPlotBoxSize(int, int);
      void setAlarmViewportUnits(AlarmRangeUnits);
      void setAlarmViewportScreenBoxSize(int, int);
      void setAlarmViewportDnBoxSize(double, double);

    public slots:
      void forgetCubes();

    protected slots:
      void colorPlot();
      void showHideContour();

    private:
      bool isXCube(MdiCubeViewport *vp) const;
      bool isYCube(MdiCubeViewport *vp) const;

      void mouseMoveEvent(QMouseEvent *e);
      void mouseLeaveEvent(QMouseEvent *e);
      void updateContourPen();

    private slots:
      void configureAlarming();

    private:
      //! This is the scatter plot's Qwt plot item
      QwtPlotSpectrogram *m_spectrogram;

      //! The action for switching the scatter plot from B/W to color.
      QPointer<QAction> m_colorize;
      //! The action for switching on and off contour lines
      QPointer<QAction> m_contour;

      //! The cube associated with the X-Axis DN values
      Cube *m_xAxisCube;
      //! The cube associated with the Y-Axis DN values
      Cube *m_yAxisCube;
      //! The DN range of the X-Axis Cube to be alarmed when painting
      QPair<double, double> m_xCubeDnAlarmRange;
      //! The DN range of the Y-Axis Cube to be alarmed when painting
      QPair<double, double> m_yCubeDnAlarmRange;
      //! The band on the X-Axis cube used for the scatter plot
      int m_xAxisCubeBand;
      //! The band on the Y-Axis cube used for the scatter plot
      int m_yAxisCubeBand;
      //! The sample range (1-based inclusive) of data used for the scatter plot
      QwtInterval m_sampleRange;
      //! The line range (1-based inclusive) of data used for the scatter plot
      QwtInterval m_lineRange;

      //! Alarm onto plot... aka alarm viewport->plot
      bool m_alarmPlot;
      //! Alarm onto viewport... aka alarm plot->viewport
      bool m_alarmViewport;

      //! Alarm viewport->plot viewport sample box size
      int m_alarmPlotSamples;
      //! Alarm viewport->plot viewport line box size
      int m_alarmPlotLines;

      //! Alarm plot->viewport current units
      AlarmRangeUnits m_alarmViewportUnits;
      //! Alarm plot->viewport X (screen pixels) box size
      int m_alarmViewportScreenWidth;
      //! Alarm plot->viewport Y (screen pixels) box size
      int m_alarmViewportScreenHeight;
      //! Alarm plot->viewport X (Cube DN) box size
      double m_alarmViewportXDnBoxSize;
      //! Alarm plot->viewport Y (Cube DN) box size
      double m_alarmViewportYDnBoxSize;
  };
}

#endif

