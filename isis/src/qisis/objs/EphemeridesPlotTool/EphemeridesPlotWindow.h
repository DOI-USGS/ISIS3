#ifndef EphemeridesPlotWindow_h
#define EphemeridesPlotWindow_h

#include "PlotWindow.h"

class QDockWidget;

namespace Isis {

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class EphemeridesPlotWindow : public PlotWindow {
      Q_OBJECT

    public:
      EphemeridesPlotWindow(QString title, QWidget *parent);
      void addRotation(CubePlotCurve *curve);
  };
};

#endif
