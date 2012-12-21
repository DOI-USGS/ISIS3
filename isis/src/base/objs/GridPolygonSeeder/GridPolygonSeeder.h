#ifndef GridPolygonSeeder_h
#define GridPolygonSeeder_h
/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2010/05/05 21:24:01 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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

      const bool SubGrid() {
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
