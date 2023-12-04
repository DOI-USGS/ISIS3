#ifndef LimitPolygonSeeder_h
#define LimitPolygonSeeder_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <geos/geom/Point.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Polygon.h>

#include "PolygonSeeder.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Seed points using a grid
   *
   * This class seeds the polygons with Control Points by creating a grid
   *  centered on the polygon. For each grid square, if it contains any overlap, a
   *  box is then created within the grid square, surrounding the valid data. The
   *  point checked is the center of this box, and if this point is within the
   *  overlap polygon then this point is returned, otherwise the grid square does
   *  not have a point.
   *
   * @ingroup PatternMatching
   *
   * @author  2008-04-21 Steven Lambright
   *
   * @internal
   *   @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
   *   @history 2008-11-12 Steven Lambright - Fixed documentation
   *   @history 2008-11-25 Steven Lambright - Added error checking
   *   @history 2008-12-23 Steven Lambright - Fixed problem with finding points in
   *            polygons that caused this algorithm to miss some.
   *   @history 2009-08-05 Travis Addair - Encapsulated group
   *            creation for seed definition group
   *   @history 2010-04-15 Eric Hyer - Now updates parent's invalidInput
   *                                   variable
   *   @history 2010-04-20 Christopher Austin - adapted for generic/unitless
   *                                            seeding
   */
  class LimitPolygonSeeder : public PolygonSeeder {
    public:
      LimitPolygonSeeder(Pvl &pvl);

      //! Destructor
      virtual ~LimitPolygonSeeder() {};

      std::vector<geos::geom::Point *> Seed(const geos::geom::MultiPolygon *mp);

      virtual PvlGroup PluginParameters(QString grpName);

    protected:
      virtual void Parse(Pvl &pvl);

    private:
      geos::geom::Geometry *GetMultiPolygon(double dMinX, double dMinY,
                                            double dMaxX, double dMaxY,
                                            const geos::geom::MultiPolygon &orig);
      int p_majorAxisPts; //!< Number of points to place on major axis
      int p_minorAxisPts; //!< Number of points to place on minor axis
  };
};

#endif
