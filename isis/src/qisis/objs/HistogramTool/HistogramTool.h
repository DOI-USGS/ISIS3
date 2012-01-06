#ifndef HistogramTool_h
#define HistogramTool_h

#include "PlotTool.h"


// FIXME: remove this include
#include "RubberBandTool.h"


class QAction;
class QWidget;

namespace Isis {
  class Brick;
}


namespace Isis {
  class HistogramItem;
  class HistogramToolWindow;
  class MdiCubeViewport;

  /**
  * @brief Tool for histograms
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Noah Hilt
  *
  * @internal
  *  @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
  */
  class HistogramTool : public PlotTool {
      Q_OBJECT

    public:
      HistogramTool(QWidget *parent);
      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

    protected:
      //!< Returns the menu name.
      QString menuName() const {
        return "&Options";
      };
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);
      QAction *p_autoScale;//!< Auto scale the plot
      void enableRubberBandTool();
      void updateTool();

    protected slots:
      void createWindow();
      void rubberBandComplete();

    public slots:
      void changePlot();
      void updateViewPort(MdiCubeViewport *);
      void copyCurve(PlotCurve *);
      void copyCurve();
      void pasteCurve(PlotWindow *);
      void pasteCurveSpecial(PlotWindow *);
      void showPlotWindow();

    private slots:
      void newPlotWindow();

    private:
      void setupPlotCurves();

      QWidget *p_parent; //!< parent widget
      HistogramToolWindow *p_histToolWindow;//!< Plot Tool Window Widget

      HistogramItem *p_copyCurve;//!< Plot curve for copying curves
      HistogramItem *p_histCurve;//!< Histogram Item
      PlotToolCurve *p_cdfCurve;//!< Plot curve for
      //
      QAction *p_action;//!< Plot tool's action

      bool p_scaled;//!< Has the plot been scaled?

      QList <QColor> p_colors;//!< List of colors
      QList<HistogramToolWindow *> p_plotWindows;//!< List of all plot windows
      RubberBandComboBox *p_rubberBand;//!< Rubber band combo box

      int p_color;//!< Keeps track of which color we are at
  };
};

#endif
