/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/07/15 16:52:37 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                      

#include "QHistogram.h"
#include "Histogram.h"
#include "iException.h"
#include <QFileDialog>
#include <QPrintDialog>
#include <QPainter>
#include <QPrinter>
#include <QMessageBox>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QLabel>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>

using namespace std;
namespace Qisis {


  /**
   *  Constructs a QHistogram object with default titles
   * 
   * 
   * @param parent 
   */
  QHistogram::QHistogram (QWidget *parent) : QwtPlot(parent){
    p_zoomer = new QwtPlotZoomer(this->canvas());
    setTitle("Histogram Plot");

    QwtText *leftLabel = new QwtText("Frequency",QwtText::PlainText);
    leftLabel->setColor(Qt::red);
    QFont font = leftLabel->font();
    font.setPointSize(13);
    font.setBold(true);
    leftLabel->setFont(font);
    setAxisTitle(QwtPlot::yLeft,*leftLabel);

    QwtText *rtLabel = new QwtText("Percentage");
    rtLabel->setColor(Qt::blue);
    rtLabel->setFont(font);
    setAxisTitle(QwtPlot::yRight,*rtLabel);

    setAxisTitle(QwtPlot::xBottom,"Pixel Value (DN)");

    setAxisScale(QwtPlot::yRight,0,100);
    enableAxis(QwtPlot::yRight,true);
    setCanvasBackground(Qt::white);
  }


 /** 
  * Plots the given Isis Histogram in the plot window
  * 
  * @param hist The Isis Histogram to plot
  */
  void QHistogram::Load (Isis::Histogram &hist) {
    p_histCurve = new QwtPlotCurve();
    p_histCurve->setStyle(QwtPlotCurve::Lines);

    p_cdfCurve = new QwtPlotCurve();
    p_cdfCurve->setStyle(QwtPlotCurve::Lines);

    //Transfer data from histogram to the plotcurve
    vector<double> xarray,yarray,y2array;
    double cumpct = 0.0;
    for (int i=0; i<hist.Bins(); i++) {
      if (hist.BinCount(i) > 0) {
        xarray.push_back(hist.BinMiddle(i));
        yarray.push_back(hist.BinCount(i));

        double pct = (double)hist.BinCount(i) / hist.ValidPixels() * 100.;
        cumpct += pct;
        y2array.push_back(cumpct);
      }
    }
    QPen *pen = new QPen(Qt::red);
    pen->setWidth(2);
    p_histCurve->setData(&xarray[0],&yarray[0],xarray.size());
    p_histCurve->setYAxis(QwtPlot::yLeft);
    p_histCurve->setPen(*pen);
    p_histCurve->attach(this);

    pen->setColor(Qt::blue);
    p_cdfCurve->setData(&xarray[0],&y2array[0],xarray.size());
    p_cdfCurve->setYAxis(QwtPlot::yRight);
    p_cdfCurve->setPen(*pen);
    p_cdfCurve->attach(this);

    this->replot();
    p_zoomer->setZoomBase();
  }


  /**
   * Enables mouse tracking on the plot.
   * 
   */
  void QHistogram::trackerEnabled() {
    if (p_zoomer->trackerMode() == QwtPicker::ActiveOnly) {
      p_zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    }
    else {
      p_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    }
  }


  /**
   * Hide/show the cdf curve.
   * 
   */
  void QHistogram::cdfCurveVisible() {
    p_cdfCurve->setVisible(!p_cdfCurve->isVisible());
    this->replot();
  }


  /**
   * Provide printing capabilities.
   * 
   */
  void QHistogram::printPlot() {
    // Initialize a printer
    static QPrinter *printer = NULL;
    if (printer == NULL) printer = new QPrinter;
    printer->setPageSize(QPrinter::Letter);
    printer->setColorMode(QPrinter::Color);
 
    QPrintDialog printDialog(printer,(QWidget *)parent());
    if (printDialog.exec() == QDialog::Accepted) {
      // Get display widget as a pixmap and convert to an image
      QPixmap pixmap = QPixmap::grabWidget(this);
      QImage img = pixmap.toImage();
 
      // C++ Gui Programmign with Qt, page 201
      QPainter painter(printer);
      QRect rect = painter.viewport();
      QSize size = img.size();
      size.scale(rect.size(),Qt::KeepAspectRatio);
      painter.setViewport(rect.x(),rect.y(),
                          size.width(),size.height());
      painter.setWindow(img.rect());
      painter.drawImage(0,0,img);
    }

  }


  /**
   * Allows user to save the plot to an image file.
   * 
   */
  void QHistogram::savePlot() {
    QString output =
      QFileDialog::getSaveFileName((QWidget *)parent(),
                                   "Choose output file",
                                   "./",
                                   QString("Images (*.png *.jpg *.tif)"));
    if (output.isEmpty()) return;

    QString format = QFileInfo(output).suffix();
    QPixmap pixmap = QPixmap::grabWidget(this);
    std::string formatString = format.toStdString();
    if (!pixmap.save(output,formatString.c_str())) {
      QMessageBox::information((QWidget *)parent(),"Error","Unable to save"+output);
      return;
    }
  }


  /**
   * Switches the plot background color between black and white.
   * 
   */
  void QHistogram::switchBackground() {
    QPen *pen = new QPen(Qt::white);
    if (canvasBackground() == Qt::white) {
      setCanvasBackground(Qt::black);
      p_zoomer->setRubberBandPen(*pen);
      p_zoomer->setTrackerPen(*pen);
    }
    else {
      setCanvasBackground(Qt::white);
      pen->setColor(Qt::black);
      p_zoomer->setRubberBandPen(*pen);
      p_zoomer->setTrackerPen(*pen);
    }
    replot();
  }


  /**
   * Provides help text in a dialog box.
   * 
   */
  void QHistogram::showHelp() {
    QDialog *d = new QDialog(this);
    d->setWindowTitle("Basic Help");

    QLabel *zoomLabel = new QLabel("Zoom Options:");
    QLabel *zoomIn = new 
      QLabel("  <b>Left click</b> on the mouse, drag, and release to select an area to zoom in on");
    QLabel *zoomOut = new 
      QLabel("  <b>Middle click</b> on the mouse to zoom out one level");
    QLabel *zoomReset = new 
      QLabel("  <b>Right click</b> on the mouse to clear the zoom and return to the original plot");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(zoomLabel);
    layout->addWidget(zoomIn);
    layout->addWidget(zoomOut);
    layout->addWidget(zoomReset);

    d->setLayout(layout);
    d->show();
  }
    
} // end namespace isis 

