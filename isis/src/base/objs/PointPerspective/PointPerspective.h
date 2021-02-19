#ifndef PointPerspective_h
#define PointPerspective_h
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
   * @brief PointPerspective Map Projection
   *
   * This class provides methods for the forward and inverse equations of an
   * PointPerspective map projection (for a sphere).
   *
   *
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
   * @author 2008-06-13 Tracie Sucharski
   *
   * @internal
   *   @history 2012-04-26 Jeannie Backer - Added forward declarations for Pvl
   *                           and PvlGroup.  Added includes to these classes in
   *                           the implementation file.
   *   @history 2012-06-15 Jeannie Backer - Minor modifications to comply with
   *                           some coding standards. References #928.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2016-08-28 Kelvin Rodriguez - Removed useless var=var lines causing warnigns in
   *                           clang. Part of Porting to OSX 10.11
   *   @history 2016-11-22 Tyler Wilson - Modified the XYRange() and SetGround() functions to eliminate
   *                           clipping in the output map.  Also added new tests to
   *                           the unit test to excercise the forward/reverse projection
   *                           equations.  Fixes #3879.
   */
  class PointPerspective : public TProjection {
    public:
      PointPerspective(Pvl &label, bool allowDefaults = false);
      ~PointPerspective();
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
      double m_distance;        //!< Distance fromp perspective point to planet center
      double m_sinph0;          //!< Sine of the center latitude
      double m_cosph0;          //!< Cosine of the center latitude
      double m_P;               //!< Perspective Point

  };
};
#endif
