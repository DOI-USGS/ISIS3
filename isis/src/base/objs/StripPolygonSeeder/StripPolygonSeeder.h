#ifndef StripPolygonSeeder_h
#define StripPolygonSeeder_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "geos/geom/Point.h"
#include "geos/geom/MultiPolygon.h"
#include "PolygonSeeder.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Seed points using a grid with a staggered pattern
   *
   * This class seeds the polygon with Control Points by creating a grid,
   *  centered on the overlap polygon. In each grid square two points are checked
   *  to see if they are inside the overlap polygon. One of these points lies 1/6
   *  of a grid square up and left from the grid's center point, while the other
   *  point lies 1/6 down and right. Each point found that is within the overlap
   *  polygon is returned as a point.
   *
   * @ingroup PatternMatching
   *
   * @author  2006-01-20 Stuart Sides
   *
   * @internal
   *   @history 2007-05-09 Tracie Sucharski,  Changed a single spacing value
   *                            to a separate value for x and y.
   *   @history 2008-02-29 Steven Lambright - Created SubGrid capabilities,
   *                            cleaned up Seed methods
   *   @history 2008-04-17 Steven Lambright - Fixed naming conventions for seeders
   *   @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
   *   @history 2008-11-12 Steven Lambright - Fixed documentation
   *   @history 2008-11-25 Steven Lambright - Added error checking
   *   @history 2009-02-01 Steven Lambright - Fixed problem with calculating
   *            starting position in the top left corner of the polygon
   *   @history 2009-08-05 Travis Addair - Encapsulated group
   *            creation for seed definition group
   *   @history 2010-04-15 Eric Hyer - Now updates parent's invalidInput
   *                                   variable (see PolygonSeeder)
   *   @history 2010-04-20 Christopher Austin - adapted for generic/unitless
   *                                            seeding
   *   @history 2010-05-05 Christopher Austin - Fixed major bug where the strip
   *                                            was not a strip.
   */
  class StripPolygonSeeder : public PolygonSeeder {
    public:
      StripPolygonSeeder(Pvl &pvl);

      //! Destructor
      virtual ~StripPolygonSeeder() {};

      std::vector<geos::geom::Point *> Seed(const geos::geom::MultiPolygon *mp);

      virtual PvlGroup PluginParameters(QString grpName);

    protected:
      virtual void Parse(Pvl &pvl);

    private:
      double p_Xspacing; //!<The spacing in the x direction between points
      double p_Yspacing; //!<The spacing in the y direction between points
  };
};

#endif
