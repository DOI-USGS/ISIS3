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
   *   @history 2012-01-20 Steven Lambright - Completed documentation.
   *   @history 2012-07-03 Steven Lambright - Added Meters, Kilometers to the Units enum.
   *                           References #853.
   *   @history 2014-04-15 Tracie Sucharski - Reset defaults for plots to the following:
   *                         SolidLine, Width=1, NoSymbols.  This is a temporary fix until
   *                         the defaults can be saved on a user basis.  Fixes #2062.
   */
  class PlotCurve : public QwtPlotCurve {

    public:
      /**
       * These are all the possible units for the x or y data in a plot curve.
       *   We want these in order to have type checking when moving curves
       *   around - it's theoretically possible to even utilize the right y axis
       *   automatically when you put mismatched plot curve y data into the same
       *   plot. All of this and more requires knowing your data's units.
       */
      enum Units {
        /**
         * The data units are not yet known. Please avoid using this if at all
         *   possible.
         */
        Unknown,
        /**
         * The data is a band number.
         */
        Band,
        /**
         * The data is a Cube DN value.
         */
        CubeDN,
        /**
         * The data is an elevation (in meters).
         */
        Elevation,
        /**
         * The data is a percentage (0-100).
         */
        Percentage,
        /**
         * The data is a pixel #. For example, it's the nth pixel along a line.
         */
        PixelNumber,
        /**
         * The data is in meters. For example, it's the nth meter along a line.
         */
        Meters,
        /**
         * The data is in kilometers. For example, it's the nth kilometer along a line.
         */
        Kilometers,
        /**
         * The data is a wavelength. This is usually associated with a band and
         *   comes from the band bin group.
         */
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
      void setData(QwtSeriesData<QPointF> *data);
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
