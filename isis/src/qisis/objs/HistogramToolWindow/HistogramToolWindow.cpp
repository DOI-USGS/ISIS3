#include "HistogramToolWindow.h"

#include <qwt_scale_engine.h>

namespace Qisis {
  /**
   * Constructor, creates a new HistogramToolWindow
   * 
   * @param title 
   * @param parent 
   */
  HistogramToolWindow::HistogramToolWindow(QString title,QWidget *parent) : PlotWindow(title, parent) {
    p_plot->enableAxis(QwtPlot::yRight);
    QwtText *leftLabel = new QwtText("Frequency",
                                     QwtText::PlainText);

    leftLabel->setColor(Qt::darkCyan);
    QFont font = leftLabel->font();
    font.setPointSize(13);
    font.setBold(true);
    leftLabel->setFont(font);
    p_plot->setAxisTitle(QwtPlot::yLeft, *leftLabel);

    p_dock = new QDockWidget("Histogram Info",this);
    p_dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    p_dock->setFloating(false);
    p_dock->setObjectName("DockWidget");
    p_dock->setMinimumWidth(130);
    p_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, p_dock, Qt::Vertical);

    QwtText *rtLabel = new QwtText("Percentage",
                                   QwtText::PlainText);
    rtLabel->setColor(Qt::red);
    font = rtLabel->font();
    font.setPointSize(13);
    font.setBold(true);
    rtLabel->setFont(font);
    p_plot->setAxisTitle(QwtPlot::yRight,*rtLabel);    

    setAxisLabel(QwtPlot::xBottom,"Pixel Value (DN)");

    setScale(QwtPlot::yRight,0,100);

    setPlotBackground(Qt::white);
  }


  /**
   * This method adds a PlotToolCurve to the window.
   * 
   * @param pc 
   */
  void HistogramToolWindow::add(PlotToolCurve *pc){
    PlotWindow::add(pc);
    p_plot->replot();
  }


  /**
   * Add a HistogramItem to the plot.
   * 
   * 
   * @param hi 
   */
  void HistogramToolWindow::add(HistogramItem *hi){
    hi->attach(p_plot);
    p_plot->replot();
    p_histItems.push_back(hi);
  }


  /**
   * 
   * 
   */
  void HistogramToolWindow::addViewMenu(){
    QMenu *viewMenu = new QMenu("&View");
    QAction *viewInfo = new QAction("View Info", this);
    connect(viewInfo, SIGNAL(activated()), p_dock, SLOT(show()));
    viewMenu->addAction(viewInfo);
    p_mainWindow->menuBar()->addMenu(viewMenu);
  }

  /** 
   * This class needs to know which viewport the user is looking 
   * at so it can appropriately draw in the band lines. 
   * 
   * @param cvp
   */
  void HistogramToolWindow::setViewport(CubeViewport *cvp){
    if(cvp == NULL) return;
    p_cvp = cvp;
  }

}
