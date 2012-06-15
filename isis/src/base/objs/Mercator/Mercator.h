#ifndef Mercator_h
#define Mercator_h
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
   * @brief Mercator Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * Mercator map projection (for an ellipsoid). 
   *  
   * The Mercator projection is cylindrical and conformal, preserving angles and
   * shapes of small objects and distorting the shape of large objects. The 
   * cylinder wraps the planet along the equator, with the poles at 
   * infinity. Latitudes and longitudes are straight lines, crossing at right 
   * angles. However, latitudes are unequally spaced and longitudes are equally 
   * spaced. 
   *  
   * The code was converted to C++ from the C version of the USGS General 
   * Cartographic Transformation Package (GCTP). This class inherits Projection 
   * and provides the two virtual methods SetGround (forward) and SetCoordinate 
   * (inverse) and a third virtual method, XYRange, for obtaining projection 
   * coordinate coverage for a latitude/longitude window. 
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *
   * @ingroup MapProjection
   *
   * @author 2005-02-25 Elizabeth Ribelin
   *
   * @internal
   *   @history 2005-03-18 Elizabeth Ribelin - added TrueScaleLatitude method to
   *                           class and tested in unitTest
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version,
   *                           IsEquatorialCylindrical methods
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file.  Moved
   *                           Name, Version, IsEquatorialCylindrical, and
   *                           TrueScaleLatitude to the implementation file.
   *                           Minor modifications to comply with some coding
                               standards. References #928.
   */
  class Mercator : public Projection {
    public:
      Mercator(Pvl &label, bool allowDefaults = false);
      ~Mercator();
      bool operator== (const Projection &proj);

      std::string Name() const;
      std::string Version() const;
      double TrueScaleLatitude() const;
      bool IsEquatorialCylindrical();

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerLatitude;  //!< The center latitude for the map projection
      double m_scalefactor;      //!< Scaling factor
  };
};

#endif
