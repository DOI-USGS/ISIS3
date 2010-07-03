
#ifndef PlotCurve_h
#define PlotCurve_h


/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2008/10/07 16:30:34 $
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>

namespace Qisis {
  class PlotCurve : public QwtPlotCurve {
 
    public:
      PlotCurve();
      void attachSymbols(QwtPlot *plot);
      void copyCurveProperties(const PlotCurve *pc);
      void detach();
      bool isSymbolVisible() const;
      QColor symbolColor() const;
      QwtSymbol symbolStyle() const;

      void setColor(QColor c);
      void setData(const QwtData &data);
      void setData(const double *xData, const double *yData, int size);
      void setPen(const QPen &pen);
      void setSymbolColor(const QColor &c);
      void setSymbolStyle(QwtSymbol::Style style);
      void setSymbolVisible(bool visible);   

      bool p_markerIsVisible;//!< Are the markers visible?

  protected:
      QwtSymbol p_symbolStyle; //!< Plot symbols
      QList<QwtPlotMarker *> p_plotMarkers;//!< List of the plot markers
      QPen p_markerPen;//!< Pen used to draw plot line and markers
      
   };
};
#endif
