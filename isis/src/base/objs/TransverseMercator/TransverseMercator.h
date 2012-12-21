#ifndef TransverseMercator_h
#define TransverseMercator_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
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

#include "Projection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief TransverseMercator Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * TransverseMercator map projection (for a sphere or ellipsoid). 
   *  
   * The Transverse Mercator projection is conformal and cylindrical 
   * (transverse), preserving angles and shapes of small objects and distorting 
   * the shape of large objects.  The cylinder wraps along a center longitude. 
   * The equator, center longitude, and longitudes 90 degrees from the center 
   * longitude are all represented as straight lines. Any other latitude or 
   * longitude is represented by a complex curve.  The true scale of the 
   * projection is found along the center longitude or along the other two 
   * longitudes that are represented as lines. 
   *  
   * The code was converted to C++ from the C version of the USGS General 
   * Cartographic Transformation Package (GCTP). This class inherits Projection 
   * and provides the two virtual methods SetGround (forward) and SetCoordinate 
   * (inverse) and a third virtual method, XYRange, for obtaining projection 
   * coordinate coverage for a latitude/longitude window. Please see the 
   * Projection class for a full accounting of all the methods available. 
   *
   * @ingroup MapProjection
   *
   * @author 2005-03-18 Elizabeth Ribelin
   *
   * @internal
   *   @history 2005-09-06 Elizabeth Ribelin - Fixed bug in radians/degrees
   *                           conversions
   *   @history 2005-12-02 Elizabeth Miller - Fixed bug in ellipsiod lat/lon
   *                           to x/y conversion
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
   *   @history 2012-05-03 Jeannie Backer - Added documentation.  Moved
   *                           inclusion of Constants.h to implemenatation file.
   *   @history 2012-06-15 Jeannie Backer - Added more documentation. Moved
   *                           Name and Version methods to the implementation
   *                           file. Minor modifications to comply with some
   *                           coding standards. References #928.
   */
  class TransverseMercator : public Projection {
    public:
      TransverseMercator(Pvl &label, bool allowDefaults = false);
      ~TransverseMercator();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerLatitude;  //!< The center latitude for the map projection
      double m_scalefactor;     //!< Scale Factor for the projection
      double m_eccsq;           //!< Eccentricity Squared
      double m_esp; /**< Snyder's (e')^2 variable from equation (8-12) on page 
                         61.  If the target body is spherical, this constant is 
                         set to 0. Otherwise, it is 
                         (eccentricity)^2 / (1 - (eccentricity)^2).**/
      double m_e0;  /**< Eccentricity Constant: 
                         e0 = 1 - e^2/4 * (1 + 3e^2/16 * (3 + 5e^2/4))
                         estimates the value 
                         e0 = 1 - e^2/4 - 3e^4/64 - 5e^6/256 - ... **/
      double m_e1;  /**< Eccentricity Constant: 
                         e1 = 3e^2/8 * (1.0 + e^2/4 * (1.0 + 15e^2/32))
                         estimates the value 
                         e1 = 3e^2/8 + 3e^4/32 + 45e^6/1024 + ...**/
      double m_e2;  /**< Eccentricity Constant: e2 = 15e^4/256 * (1 + 3e^2/4))
                         estimates the value 
                         e2 = 15e^4/256 + 45e^6/1024 + ...**/
      double m_e3;  /**< Eccentricity Constant: e3 = 35e^6/3072 
                         estimates the value e3 = 35e^6/3072 + ...**/
      double m_ml0; /**< Distance along the meridian from the equator
                                    to the center latitude.*/
      bool m_sph;   /**< Flag set to true if sphere, and false if ellipsiod.*/
  };
};
#endif

