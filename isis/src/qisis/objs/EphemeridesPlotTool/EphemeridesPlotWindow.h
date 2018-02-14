#ifndef EphemeridesPlotWindow_h
#define EphemeridesPlotWindow_h

#include "PlotWindow.h"

class QDockWidget;

namespace Isis {

  /**
   * A plot window to display ephemerides, position and rotation data, from a
   * cube. This window is expected to be used by the EphemeridesPlotTool class.
   * 
   * @author 2017-07-17 Jesse Mapel
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
