#include "PlotToolWindow.h"

#include <QString>

#include "Cube.h"
#include "Pvl.h"
#include "PvlGroup.h"



using namespace Isis;

namespace Qisis {


  /**
  * This class was developed specifically to be used in conjuntion
  * with the PlotTool.  i.e. this is the PlotWindow for the
  * PlotTool.  This class handles items on the PlotWindow unique
  * to the PlotTool such as the vertical lines drawn on the plot
  * area called the band lines.
  */
  PlotToolWindow::PlotToolWindow(QString title, QWidget *parent) :
    Qisis::PlotWindow(title, parent) {

    p_grayBandLine = NULL;
    p_redBandLine = NULL;
    p_greenBandLine = NULL;
    p_blueBandLine = NULL;
    p_cvp = NULL;
    p_plotType = NULL;
    p_wavelengths = NULL;
    p_stdDevArray = NULL;

    p_plotType = new QString;
    p_wavelengths = new PvlKeyword;
    p_stdDevArray = new QVector< double >;

    /*Plot line draws the vertical line(s) to indicate
    which band(s) the user is looking at in the cube viewport*/
    p_grayBandLine = new QwtPlotMarker();
    p_redBandLine = new QwtPlotMarker();
    p_greenBandLine = new QwtPlotMarker();
    p_blueBandLine = new QwtPlotMarker();

    p_grayBandLine->setLineStyle(QwtPlotMarker::LineStyle(2));
    p_redBandLine->setLineStyle(QwtPlotMarker::LineStyle(2));
    p_greenBandLine->setLineStyle(QwtPlotMarker::LineStyle(2));
    p_blueBandLine->setLineStyle(QwtPlotMarker::LineStyle(2));

    QPen *pen = new QPen(Qt::white);
    pen->setWidth(1);
    p_grayBandLine->setLinePen(*pen);
    pen->setColor(Qt::red);
    p_redBandLine->setLinePen(*pen);
    pen->setColor(Qt::green);
    p_greenBandLine->setLinePen(*pen);
    pen->setColor(Qt::blue);
    p_blueBandLine->setLinePen(*pen);

    p_grayBandLine->hide();
    p_redBandLine->hide();
    p_greenBandLine->hide();
    p_blueBandLine->hide();

    p_grayBandLine->attach(this->p_plot);
    p_redBandLine->attach(this->p_plot);
    p_greenBandLine->attach(this->p_plot);
    p_blueBandLine->attach(this->p_plot);

    /*Setting the default to all lines hidden and all symbols visible.*/
    this->hideAllCurves();
  }


  PlotToolWindow::~PlotToolWindow() {
    if(p_plotType) {
      delete p_plotType;
      p_plotType = NULL;
    }

    if(p_wavelengths) {
      delete p_wavelengths;
      p_wavelengths = NULL;
    }

    if(p_stdDevArray) {
      delete p_stdDevArray;
      p_stdDevArray = NULL;
    }

  }


  /**
   * This class needs to know which viewport the user is looking
   * at so it can appropriately draw in the band lines.
   *
   * @param cvp
   */
  void PlotToolWindow::setViewport(CubeViewport *cvp) {
    if(cvp == NULL) return;
    p_cvp = cvp;

  }

  /**
   * This method actually draws in the vertical band line(s) on
   * the plot area.
   *
   */
  void PlotToolWindow::drawBandMarkers() {
    if(p_cvp == NULL) return;
    if(!p_markersVisible) return;

    int redBand = 0, greenBand = 0, blueBand = 0, grayBand = 0;

    Cube *cube = p_cvp->cube();
    Pvl &pvl = *cube->getLabel();

    if(pvl.FindObject("IsisCube").HasGroup("BandBin")) {
      PvlGroup &bandBin = pvl.FindObject("IsisCube").FindGroup("BandBin");
      if(bandBin.HasKeyword("Center")) {
        *p_wavelengths = bandBin.FindKeyword("Center");
      }
    }

    if(p_cvp->isColor()) {
      redBand = p_cvp->redBand();
      greenBand = p_cvp->greenBand();
      blueBand = p_cvp->blueBand();
    }
    else {
      grayBand = p_cvp->grayBand();
    }


    /*This is were we need to set the x value to the band number.*/
    if(grayBand > 0) {
      if(*p_plotType == "Wavelength") {
        p_grayBandLine->setXValue((*p_wavelengths)[grayBand-1]);

      }
      else {
        p_grayBandLine->setXValue(grayBand);
      }
      p_grayBandLine->show();
    }
    else {
      p_grayBandLine->hide();
    }

    if(redBand > 0) {

      if(*p_plotType == "Wavelength") {
        p_redBandLine->setXValue((*p_wavelengths)[redBand-1]);
      }
      else {
        p_redBandLine->setXValue(redBand);
      }
      p_redBandLine->show();

    }
    else {
      p_redBandLine->hide();
    }

    if(greenBand > 0) {

      if(*p_plotType == "Wavelength") {
        p_greenBandLine->setXValue((*p_wavelengths)[greenBand-1]);
      }
      else {
        p_greenBandLine->setXValue(greenBand);
      }
      p_greenBandLine->show();

    }
    else {
      p_greenBandLine->hide();
    }

    if(blueBand > 0) {

      if(*p_plotType == "Wavelength") {
        p_blueBandLine->setXValue((*p_wavelengths)[blueBand-1]);
      }
      else {
        p_blueBandLine->setXValue(blueBand);
      }
      p_blueBandLine->show();

    }
    else {
      p_blueBandLine->hide();
    }

    this->p_plot->replot();

  }

  /**
   *
   *
   * @param visible
   */
  void PlotToolWindow::setBandMarkersVisible(bool visible) {
    p_markersVisible = visible;
  }

  /**
   *
   *
   *
   * @return bool
   */
  bool PlotToolWindow::bandMarkersVisible() {
    return p_markersVisible;
  }

  /**
   * This method is called from PlotTool.  This enables the users
   * to hide or show the vertical line(s) on the plot which
   * represent the color bands or the black/white band.
   *
   */
  void PlotToolWindow::showHideLines() {
    if(p_cvp->isColor()) {
      p_blueBandLine->setVisible(!p_markersVisible);
      p_redBandLine->setVisible(!p_markersVisible);
      p_greenBandLine->setVisible(!p_markersVisible);
    }
    else {
      p_grayBandLine->setVisible(!p_markersVisible);
    }

    p_markersVisible = !p_markersVisible;

    this->p_plot->replot();
  }

  /**
   *
   *
   * @param plotType
   */
  void PlotToolWindow::setPlotType(QString plotType) {
    *p_plotType = plotType;

  }

  /**
   *
   *
   * @param autoScale
   */
  void PlotToolWindow::setAutoScaleOption(bool autoScale) {
    p_autoScale = autoScale;
  }

  /**
   * Fills in the table with the data from the current curves
   * in the plotWindow  -- overridden function from PlotWindow
   */
  void PlotToolWindow::fillTable() {
    if(p_tableWindow == NULL) return;
    PlotWindow::fillTable();
  }


  /**
   * Gives us access to the standard deviation array so we can
   * display it in the table.
   *
   * @param stdDevArray
   */
  void PlotToolWindow::setStdDev(QVector< double > stdDevArray) {
    *p_stdDevArray = stdDevArray;
  }

}




