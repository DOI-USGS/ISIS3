#ifndef HistogramTool_h
#define HistogramTool_h

#include "AbstractPlotTool.h"

class QAction;
class QWidget;


namespace Isis {
  class Brick;
  class CubePlotCurve;
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
  class HistogramTool : public AbstractPlotTool {
      Q_OBJECT

    public:
      HistogramTool(QWidget *parent);

    protected:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      void detachCurves();
      PlotWindow *createWindow();
      void enableRubberBandTool();
      QAction *toolPadAction(ToolPad *pad);
      void updateTool();

    protected slots:
      void rubberBandComplete();

    public slots:
      void refreshPlot();

    private:
      void validatePlotCurves();

      HistogramToolWindow *m_histToolWindow;//!< Plot Tool Window Widget

      QPointer<HistogramItem> m_frequencyItem;
      QPointer<CubePlotCurve> m_percentageCurve;
      QAction *m_action;
      QPointer<RubberBandComboBox> m_rubberBandCombo;
  };
};

#endif
