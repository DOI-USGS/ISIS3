#ifndef HistogramWidget_h
#define HistogramWidget_h

#include "Cube.h"
#include "Stretch.h"
#include "Histogram.h"

#include <QWidget>
#include <QStackedWidget>
#include <QSlider>
#include <QLineEdit>
#include <QDoubleValidator>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>

#include "HistogramItem.h"

class QwtPlotZoomer;

namespace Qisis {
  /**
   * @brief Histogram widget used by AdvancedStretchTool
   *
   *
   * The HistogramWidget displays a given histogram and stretch in a graph and
   * contains inputs for changing the min/max of the histogram.
   *
   * @ingroup Visualization Tools
   *
   * @author 2009-05-01 Noah Hilt
   *
   */
  class HistogramWidget : public QwtPlot {
      Q_OBJECT

    public:
      HistogramWidget(const QString title, const QColor histColor = Qt::gray, const QColor stretchColor = Qt::darkGray);
      void setHistogram(const Isis::Histogram &hist);
      void setStretch(Isis::Stretch stretch);

      void clearStretch();

      /**
       * Histograms have preferred sizes that keeps them all the same
       * regardless of the contained data. This causes that.
       *
       * Prefer 1:1
       *
       * @param w
       *
       * @return int
       */
      int heightForWidth(int w) const {
        return w;
      }

    private slots:

    private:
      HistogramItem *p_histCurve;  //!< The histogram curve
      QwtPlotCurve *p_stretchCurve;  //!< The stretch curve
      QwtPlotZoomer *p_zoomer; //!< This allows for zooming in/out

      double p_min; //!< The minimum value the histogram's minimum can be set to
      double p_max; //!<  The maximum value the histogram's maximum can be set to
  };
};

#endif
