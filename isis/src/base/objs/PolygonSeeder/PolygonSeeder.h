#ifndef PolygonSeeder_h
#define PolygonSeeder_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/05 21:22:10 $
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

#include <string>
#include <vector>
#include <cmath>

#include "geos/geom/Point.h"
#include "geos/geom/MultiPolygon.h"

#include "Projection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  class PolygonTools;

  /**
   * This class is used as the base class for all PolygonSeeder objects. The
   * class is pure virtual.
   *
   * @ingroup PatternMatching
   *
   * @author 2006-01-20 Stuart Sides
   *
   * @internal
   *   @history 2008-08-18 Christopher Austin - Upgraded to
   *                           geos3.0.0, removed Chip.h include, fixed ifndef
   *   @history 2009-08-05 Travis Addair - Encapsulated group
   *                           creation for seed definition group
   *   @history 2009-04-15 Eric Hyer - Now stores invalid input.  Added Copy
   *                           constructor, destructor, and assignment operator
   *   @history 2010-04-20 Christopher Austin - adapted for generic/unitless
   *                           seeding
   *  @history 2012-04-17 Jeannie Backer - Added forward declaration for
   *                          PvlObject and ordered includes in the
   *                          implementation file. Added documentation.
   *                          Moved Algorithm method from header file to
   *                          implementation file.
   */
  class PolygonSeeder {
    public:
      PolygonSeeder(Pvl &pvl);
      PolygonSeeder(const PolygonSeeder &other);
      virtual ~PolygonSeeder();

      /**
       * Pure virtual seed method.
       *
       * @param mp The MultiPolygon object from the geos::geom library.
       * @return @b std::vector<geos::geom::Point*> A vector of Point objects
       *                from the geos::geom library.
       */
      virtual std::vector<geos::geom::Point *>
          Seed(const geos::geom::MultiPolygon *mp) = 0;

      double MinimumThickness();
      double MinimumArea();
      QString Algorithm() const;

      virtual PvlGroup PluginParameters(QString grpName);
      Pvl InvalidInput();

      const PolygonSeeder &operator=(const PolygonSeeder &other);

    protected:
      virtual void Parse(Pvl &pvl);
      QString StandardTests(const geos::geom::MultiPolygon *multiPoly,
                                const geos::geom::Envelope *polyBoundBox);

    protected:
      Pvl *invalidInput; /**< The Pvl passed in by the constructor minus what
                              was used.*/

    private:
      QString p_algorithmName; /**< The value for the 'Name' Keyword in the
                                        PolygonSeederAlgorithm group of the Pvl
                                        that is passed into the constructor.*/
      double p_minimumThickness;   /**< The value for the 'MinimumThickness'
                                        Keyword in the PolygonSeederAlgorithm
                                        group of the Pvl that is passed into
                                        the constructor*/
      double p_minimumArea;        /**< The value for the 'MinimumArea' Keyword
                                        in the PolygonSeederAlgorithm group of
                                        the Pvl that is passed into the
                                        constructor*/

  };
};

#endif
