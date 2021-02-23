#ifndef QHistogram_h
#define QHistogram_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Histogram.h"
#include <QWidget>
#include <QMenu>
#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_curve.h>

namespace Isis {
  /**
   * @brief Plot Histograms
   *
   * This class is used to plot histograms.  It is a utility class for the hist
   * application.
   *
   * @ingroup Utility
   *
   * @author 2006-12-21 Elizabeth Miller
   *
   * @internal
   *   @history 2016-09-14 Ian Humphrey - Modified printPlot() and savePlot() - replaced deprecated
   *                           static QPixmap::grabWidget with QWidget::grab. References #4304.
   */
  class QHistogram: public QwtPlot {
      Q_OBJECT

    public:
      QHistogram(QWidget *parent = NULL);

      //! Destroys the QHistogram object
      ~QHistogram() {};

      void Load(Histogram &hist);

    public slots:
      void trackerEnabled();
      void cdfCurveVisible();
      void printPlot();
      void savePlot();
      void switchBackground();
      void showHelp();

    private:
      QwtPlotZoomer *p_zoomer; //!< Plot Zoomer
      QwtPlotCurve *p_histCurve; //!< Historgram plot curve
      QwtPlotCurve *p_cdfCurve; //!< CDF plot curve
  };
};

#endif
