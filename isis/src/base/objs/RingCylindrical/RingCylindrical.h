#ifndef RingCylindrical_h
#define RingCylindrical_h
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
   * @brief Ring Cylindrical Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * Ring Cylindrical map projection (for a sphere).
   *
   * The Ring Cylindrical projection is an
   *
   * Please see the Projection class for a full accounting of all the methods
   * available.
   *
   * @ingroup MapProjection
   * @see Equirectangular
   *
   * @author 2013-03-10 Debbie A. Cook
   *
   * @internal
   *   @history 2013-05-09 Jeannie Backer - Added comments and documentation.
   *            References #775.
   *   @history 2016-08-28 Kelvin Rodriguez - Removed several redundant var=var lines
   *            causing warnings in clang. Part of porting to OS X 10.11.
   */
  // or Rectilinear projection?? scale azimuth with 1/(2*pi) * radius maybe
  class RingCylindrical : public RingPlaneProjection {
    public:
      RingCylindrical(Pvl &label, bool allowDefaults = false);
      ~RingCylindrical();
      bool operator==(const Projection &proj);

      QString Name() const;
      QString Version() const;
      bool IsEquatorialCylindrical();
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
      double m_centerRingLongitude; /**< The center ring longitude (azimuth) for the map projection,
                                         in radians*/
      double m_centerRingRadius;    //!< The center ring radius for the map projection
  };
};

#endif
