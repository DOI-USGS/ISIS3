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
   */
  class ScatterPlotWindow : public PlotWindow {
      Q_OBJECT

    public:
      enum AlarmRangeUnits {
        CubeUnits,
        ScreenUnits
      };

      ScatterPlotWindow(QString title,
                        Cube *xAxisCube, int xAxisBand, int xAxisBinCount,
                        Cube *yAxisCube, int yAxisBand, int yAxisBinCount,
                        QwtDoubleRange sampleRange, QwtDoubleRange lineRange,
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
      QwtPlotSpectrogram *m_spectrogram;

      //! The action for switching the scatter plot from B/W to color.
      QPointer<QAction> m_colorize;
      QPointer<QAction> m_contour;

      Cube *m_xAxisCube;
      Cube *m_yAxisCube;
      QPair<double, double> m_xCubeDnAlarmRange;
      QPair<double, double> m_yCubeDnAlarmRange;
      int m_xAxisCubeBand;
      int m_yAxisCubeBand;
      QwtDoubleRange m_sampleRange;
      QwtDoubleRange m_lineRange;

      //! Alarm onto plot
      bool m_alarmPlot;
      //! Alarm onto viewport
      bool m_alarmViewport;

      int m_alarmPlotSamples;
      int m_alarmPlotLines;

      AlarmRangeUnits m_alarmViewportUnits;
      int m_alarmViewportScreenWidth;
      int m_alarmViewportScreenHeight;
      double m_alarmViewportXDnBoxSize;
      double m_alarmViewportYDnBoxSize;
  };
}

#endif

