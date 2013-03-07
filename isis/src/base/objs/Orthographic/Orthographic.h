#ifndef Orthographic_h
#define Orthographic_h
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
   * @brief Orthographic Map Projection
   *
   * This class provides methods for the forward and inverse equations of an
   * Orthographic map projection (for a sphere). 
   *  
   * The Orthographic projection is an azimuthal projection.  Latitudes and 
   * longitudes are ellipses, circles, or straight lines.  Only one hemisphere 
   * can be projected.  Scale is true at the point (center latitude, center 
   * longitude). 
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
   * @author 2005-03-04 Elizabeth Ribelin
   *
   * @internal
   *   @history 2005-08-22 Kris Becker - Fixed bug in XYRange method that 
   *                           computes the line/samp ranges as it was not
   *                           correctly computing limb limits
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes 
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
   *   @history 2011-01-31 Steven Lambright - Improved ability to work with
   *                           lat/lon ranges outside of what is possible to
   *                           project. Now a lat,lon range of -90 to 90, 0 to
   *                           360 works. Improved XYRange method to have a
   *                           better chance at success.
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file. Moved
   *                           Name, Version, and TrueScaleLatitude to the
   *                           implementation file. Minor modifications to
   *                           comply with some coding standards. References
   *                           #928.
   *   @history 2013-02-22 Kimberly Oyama and Debbie Cook - XYRange() and SetCoordinate()
   *                           were modified because XYRange() was returning the wrong x/y
   *                           ranges caused by a false failure in SetCoordinate(). Added
   *                           a case to the unit test to check the x/y values when the
   *                           lat/lon ranges are smaller than the possible range of the
   *                           projection (half the planet). Fixes #798.
   */
  class Orthographic : public Projection {
    public:
      Orthographic(Pvl &label, bool allowDefaults = false);
      ~Orthographic();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;
      double TrueScaleLatitude() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerLatitude;  //!< The center latitude for the map projection
      double m_sinph0;            //!< Sine of the center latitude
      double m_cosph0;            //!< Cosine of the center latitude
  };
};
#endif
