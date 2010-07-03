#ifndef HistogramToolWindow_h
#define HistogramToolWindow_h

#include "PlotWindow.h"
#include "PlotToolCurve.h"
#include "HistogramItem.h"
#include "Histogram.h"

#include <QDockWidget>

namespace Qisis {
  class HistogramToolWindow : public Qisis::PlotWindow {
    Q_OBJECT

    public:     
      HistogramToolWindow(QString title,QWidget *parent);
      void add(PlotToolCurve *pc);
      void add(HistogramItem *hi);
      void addViewMenu();
      void setViewport(CubeViewport *cvp);
      int getNumItems() { return p_histItems.size(); }
      HistogramItem *getHistItem(int index) { return p_histItems[index]; }
      QDockWidget *getDockWidget() { return p_dock; }

  public slots:

   
    private:
      CubeViewport  *p_cvp; //!< The current viewport
      QList<HistogramItem *> p_histItems;
      QDockWidget *p_dock;
  };
};

#endif
