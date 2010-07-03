#include "PlotCurve.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <iostream>

#include "PlotTool.h"
#include "SpecialPixel.h"

namespace Qisis {
  /**
   * Constructs and instance of a PlotCurve with some default
   * properties.
   */
 PlotCurve::PlotCurve ()  : QwtPlotCurve() {
              //The default is to show the symbols but not the curves.
              setVisible(false);
              setSymbolVisible(true);

              p_symbolStyle.setStyle(QwtSymbol::XCross);
              p_symbolStyle.setSize(6,6);
              p_markerPen.setColor(this->pen().color());
 }


  /**
   * This method sets the data for the curve, then sets the value
   * for the markers associated with the curve.
   * @param data
   */
  void PlotCurve::setData(const QwtData &data){
    //set the data for the curve
    this->QwtPlotCurve::setData(data); 
    //now we can set up the marker list and set the data for them.
    for(int i = 0; i<p_plotMarkers.size(); i++){
      delete p_plotMarkers[i];
    }
    p_plotMarkers.clear();
    for(unsigned int i = 0; i<data.size(); i++){
      if(data.y(i) != Isis::Null ) { 
        QwtPlotMarker *marker = new QwtPlotMarker();
        marker->setValue(data.x(i), data.y(i));
        marker->setAxis(this->xAxis(), this->yAxis());
        p_plotMarkers.push_back( marker);
        
      }
      
    }
  }


  /**
   * This method sets the data for the curve, then sets the value
   * for the markers associated with the curve.
   * @param xData
   * @param yData
   * @param size
   */
  void PlotCurve::setData(const double *xData, const double *yData, int size){
    /*set the data for the curve*/
    this->QwtPlotCurve::setData(xData, yData, size); 
    /*now we can set up the marker list and set the data for them.*/
    for(int i = 0; i<p_plotMarkers.size(); i++){
      delete p_plotMarkers[i];
    }
    p_plotMarkers.clear();
    for(int i = 0; i<size; i++){
      if(yData[i] != 0 ) {
        QwtPlotMarker *marker = new QwtPlotMarker();  
        marker->setValue(xData[i], yData[i]);
        marker->setAxis(this->xAxis(), this->yAxis());
        //marker->setVisible(true);
        //setSymbolVisible(true);
        p_plotMarkers.push_back( marker);
      } 
    }

  }


  /**
   * This method allows the user to set the color of the curve and
   * it's markers.
   * @param c
   */
  void PlotCurve::setColor(QColor c){
    this->setPen(QPen(c));
  }


  /**
   * This method removes all the curves and markers from the plot.
   */
  void PlotCurve::detach(){
    QwtPlotCurve::detach();
    for(int i = 0; i<p_plotMarkers.size(); i++){
      p_plotMarkers[i]->detach();
    }
  }


  /**
   * This method allows the users to copy all of the given curves
   * properties into the current curve.
   * @param pc
   */
  void PlotCurve::copyCurveProperties(const PlotCurve *pc){
    this->setVisible(pc->isVisible());
    this->setPen(pc->pen());
    this->setTitle(pc->title().text());
    this->setData(pc->data());
    this->setSymbolColor(pc->symbolColor());
    this->setSymbolStyle(pc->symbolStyle().style());
    this->setSymbolVisible(pc->isSymbolVisible());
  }


  /**
   * This method returns the shape of the markers.
   * 
   * @return QwtSymbol
   */
  QwtSymbol PlotCurve::symbolStyle() const {
    return p_symbolStyle;
  }


  /**
   *  This method sets the shape of the markers.
   * @param style
   */
  void PlotCurve::setSymbolStyle(QwtSymbol::Style style){
    p_symbolStyle.setStyle(style);
    for(int i = 0; i<p_plotMarkers.size(); i++){
      p_plotMarkers[i]->setSymbol(p_symbolStyle);
      p_plotMarkers[i]->setVisible(p_markerIsVisible);
    }
  }


  /**
   * This method returns the color of the curve's markers.
   * 
   * @return QColor
   */
  QColor PlotCurve::symbolColor() const {
    return p_markerPen.color();
  }


  /**
   * This method sets the color of the curve's markers.
   * @param c
   */
  void PlotCurve::setSymbolColor(const QColor &c){
    p_markerPen.setColor(c);
  }


  /**
   * This methods hides/shows the curve's markers.
   * @param visible
   */
  void PlotCurve::setSymbolVisible(bool visible){
    p_markerIsVisible = visible;
    for(int i = 0; i<p_plotMarkers.size(); i++){
      p_plotMarkers[i]->setSymbol(p_symbolStyle);
      p_plotMarkers[i]->setVisible(p_markerIsVisible);
    }
  }


  /**
   * Sets the plot pen to the passed-in pen.
   * 
   * 
   * @param pen 
   */
  void PlotCurve::setPen(const QPen &pen){
    this->QwtPlotCurve::setPen(pen);

    setSymbolColor(pen.color());
    p_symbolStyle.setPen(p_markerPen);
    for(int i = 0; i<p_plotMarkers.size(); i++){
      p_plotMarkers[i]->setSymbol(p_symbolStyle);
      p_plotMarkers[i]->setVisible(p_markerIsVisible);
    }

  }


  /**
   * This method returns a bool indicating if the  curve's markers
   * are visible.
   * 
   * @return bool
   */
  bool PlotCurve::isSymbolVisible() const {
    return p_markerIsVisible;
  }


  /**
   * This method attachs the curves's markers to the plot.
   * @param plot
   */
  void PlotCurve::attachSymbols(QwtPlot *plot){
    p_symbolStyle.setPen(p_markerPen);
    for(int i = 0; i<p_plotMarkers.size(); i++){
      p_plotMarkers[i]->setSymbol(p_symbolStyle);
      p_plotMarkers[i]->setVisible(p_markerIsVisible);
      p_plotMarkers[i]->attach(plot);
    }
  }

}
