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

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class PlotCurve : public QwtPlotCurve {

    public:
      enum Units {
        Unknown,
        Band,
        CubeDN,
        Elevation,
        Percentage,
        PixelNumber,
        Wavelength
      };

      PlotCurve(Units xUnits, Units yUnits);
      ~PlotCurve();

      void attachMarkers();

      QColor color() const;
      QwtSymbol markerSymbol() const;
      Units xUnits() const;
      Units yUnits() const;

      void setColor(const QColor &color);
      void setData(const QwtData &data);
      void setData(const double *xData, const double *yData, int size);
      void setPen(const QPen &pen);
      void setMarkerSymbol(QwtSymbol symbol);
      void setMarkerVisible(bool visible);

    protected:
      QByteArray fromByteArray(const QByteArray &classData);
      QByteArray toByteArray() const;

    private:
      PlotCurve(const PlotCurve &other);
      PlotCurve &operator=(const PlotCurve &other);

      void clearMarkers();
      void recreateMarkers();

    private:
      QColor m_color;
      QwtSymbol m_markerSymbol; //!< Marker's styles
      QList<QwtPlotMarker *> m_valuePointMarkers;
      Units m_xUnits;
      Units m_yUnits;
  };
};
#endif
