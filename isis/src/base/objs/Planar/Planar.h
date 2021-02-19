#ifndef Planar_h
#define Planar_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "RingPlaneProjection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief Planar Map Projection
   *
   * This class provides methods for the forward and inverse equations of an
   * Planar map projection.
   *
   * The Orthographic projection is an azimuthal projection.  Latitudes and
   * longitudes are ellipses, circles, or straight lines.  Only one hemisphere
   * can be projected.  Scale is true at the point (center latitude, center
   * longitude).
   *
   *
   * See the Projection class for a full accounting of all the methods
   * available.
   *
   * @ingroup MapProjection
   *
   * @author 2012-09-09 Ken Edmundson
   *
   * @internal
   *   @history 2012-08-09 Ken Edmundson - initial version
   *   @history 2012-01-20 Debbie A. Cook - Changed to use RingPlaneProjection instead of Projection.
   *                           References #775.
   *   @history 2016-08-28 Kelvin Rodriguez - Removed several redundant var=var lines
   *                           causing warnings in clang. Part of porting to OS X 10.11. 
   */
  class Planar : public RingPlaneProjection {
    public:
      Planar(Pvl &label, bool allowDefaults = false);
      ~Planar();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;
      double TrueScaleRingRadius() const;

      double CenterRingLongitude() const;
      double CenterRingRadius() const;

      bool SetGround(const double ringRadius, const double ringLongitude);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingRingRadii();
      PvlGroup MappingRingLongitudes();

  protected:

    private:
      double m_centerRingLongitude; //!< The center longitude for the map projection
      double m_centerRingRadius;    //!< The center radius for the map projection
  };
};
#endif
