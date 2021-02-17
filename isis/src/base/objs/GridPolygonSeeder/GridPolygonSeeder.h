#ifndef GridPolygonSeeder_h
#define GridPolygonSeeder_h
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
   * @brief Seed points using a grid
   *
   * This class is used to construct a grid of points inside a polygon.
   *
   * @ingroup PatternMatching
   *
   * @author  2006-01-20 Stuart Sides
   *
   * @internal
   * @history 2007-05-09 Tracie Sucharski,  Changed a single spacing value
   *                            to a separate value for x and y.
   * @history 2008-02-29 Steven Lambright - Created SubGrid capabilities,
   *                            cleaned up Seed methods
   * @history 2008-06-18 Christopher Austin - Fixed documentation errors
   * @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
   * @history 2008-11-25 Steven Lambright - Added error checking
   * @history 2008-12-05 Christopher Austin - capped the subgrid to 127x127 to
   *          prevent segfaults on too high a precision
   * @history 2000-01-30 Steven Lambright - Fixed an issue with not seeding entire
   *          polygons when a large portion of the polygon was on the right side
   *          of the envelope.
   * @history 2009-08-05 Travis Addair - Encapsulated group
   *          creation for seed definition group
   * @history 2010-04-15 Eric Hyer - Now updates parent's invalidInput variable
   *                               - included SubGrid for what PluginParameters
   *                                 returns
   * @history 2010-04-20 Christopher Austin - adapted for generic/unitless
   *                                          seeding
   */
  class GridPolygonSeeder : public PolygonSeeder {
    public:
      GridPolygonSeeder(Pvl &pvl);

      //! Destructor
      virtual ~GridPolygonSeeder() {};

      std::vector<geos::geom::Point *> Seed(const geos::geom::MultiPolygon *mp);

      bool SubGrid() {
        return p_subGrid;
      }
      virtual PvlGroup PluginParameters(QString grpName);

    protected:
      virtual void Parse(Pvl &pvl);

    private:
      std::vector<geos::geom::Point *> SeedGrid(const geos::geom::MultiPolygon *mp);

      std::vector<geos::geom::Point *> SeedSubGrid(const geos::geom::MultiPolygon *mp);


      geos::geom::Point *CheckSubGrid(const geos::geom::MultiPolygon &, const double &,
                                      const double &, const int &);

      double p_Xspacing;
      double p_Yspacing;
      bool   p_subGrid;
  };
};

#endif
