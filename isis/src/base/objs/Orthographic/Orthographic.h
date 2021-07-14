#ifndef Orthographic_h
#define Orthographic_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "TProjection.h"

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
   * @ingroup MapProjectioncisbell
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
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2013-02-22 Kimberly Oyama and Debbie Cook - XYRange() and SetCoordinate()
   *                           were modified because XYRange() was returning the wrong x/y
   *                           ranges caused by a false failure in SetCoordinate(). Added
   *                           a case to the unit test to check the x/y values when the
   *                           lat/lon ranges are smaller than the possible range of the
   *                           projection (half the planet). Fixes #798.
   *   @history 2013-07-25 Kimberly Oyama - Fixed the longitude equation for the south pole
   *                           by making x positive instead of negative. Fixes #1719.
   *   @history 2013-08-20 Kimberly Oyama - Removed code for the special case, clat = 0,
   *                           in the constructor to give the correct longitude range
   *                           when clat = 0. Added a range check in SetCoordinate()
   *                           to make sure m_longitude is in the correct domain.
   *                           Fixes #1471.
   *   @history 2016-08-28 Kelvin Rodriguez - Removed several redundant var=var lines
   *                           causing warnings in clang. Part of porting to OS X 10.11. 
   */
  class Orthographic : public TProjection {
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
