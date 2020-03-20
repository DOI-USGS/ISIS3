/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2010/05/05 21:31:14 $
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

#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "PolygonTools.h"

#include "StripPolygonSeeder.h"

namespace Isis {

  /**
   * @brief Construct a StripPolygonSeeder algorithm
   *
   *
   * @param pvl  A Pvl object that contains a valid polygon point
   * seeding definition
   */
  StripPolygonSeeder::StripPolygonSeeder(Pvl &pvl) : PolygonSeeder(pvl) {
    Parse(pvl);
  };


  /**
   * @brief Seed a polygon with points
   *
   * Seed the supplied polygon with points in a staggered pattern. The spacing
   * is determined by the PVL group "PolygonSeederAlgorithm"
   *
   * @param lonLatPoly The polygon to be seeded with
   *                  points.
   *
   * @return A vector of points which have been
   * seeded into the polygon. The caller assumes responsibility for deleteing
   *  these.
   *
   *  @internal
   *   @history 2007-05-09 Tracie Sucharski Changed a single spacing value
   *                            to a separate value for x and y.
   *   @history 2008-06-18 Steven Lambright Fixed documentation
   *
   */
  std::vector<geos::geom::Point *> StripPolygonSeeder::Seed(const geos::geom::MultiPolygon *multiPoly) {

    // Storage for the points to be returned
    std::vector<geos::geom::Point *> points;

    // Create some things we will need shortly
    const geos::geom::Envelope *polyBoundBox = multiPoly->getEnvelopeInternal();

    // Call the parents standardTests member
    QString msg = StandardTests(multiPoly, polyBoundBox);
    if(!msg.isEmpty()) {
      return points;
    }

    // Do strip seeder specific tests to make sure this poly should be seeded
    // (none for now)

    // Starting at the centroid of the xy polygon populate the polygon with
    // staggered points with the requested spacing
    geos::geom::Point *centroid = multiPoly->getCentroid();
    double centerX = centroid->getX();
    double centerY = centroid->getY();
    delete centroid;

    int xStepsToCentroid = (int)((centerX - polyBoundBox->getMinX()) / p_Xspacing + 0.5);
    int yStepsToCentroid = (int)((centerY - polyBoundBox->getMinY()) / p_Yspacing + 0.5);
    double dRealMinX = centerX - (xStepsToCentroid * p_Xspacing);
    double dRealMinY = centerY - (yStepsToCentroid * p_Yspacing);
    double dDeltaXToReal = p_Xspacing * 1.0 / 6.0;
    double dDeltaYToReal = p_Yspacing * 1.0 / 6.0;

    for(double y = dRealMinY; y <= polyBoundBox->getMaxY(); y += p_Yspacing) {
      //printf("Grid Line,%.10f,%.10f,Through,%.10f,%.10f\n",dRealMinX, y, xyBoundBox->getMaxX(), y);
      for(double x = dRealMinX; x <= polyBoundBox->getMaxX(); x += p_Xspacing) {
        geos::geom::Coordinate c(x + dDeltaXToReal, y + dDeltaYToReal);
        geos::geom::Point *p = Isis::globalFactory->createPoint(c);
        if(p->within(multiPoly)) {
          points.push_back(Isis::globalFactory->createPoint(c));
        }

        geos::geom::Coordinate c2(x - dDeltaXToReal, y - dDeltaYToReal);
        p = Isis::globalFactory->createPoint(c2);
        if(p->within(multiPoly)) {
          points.push_back(Isis::globalFactory->createPoint(c2));
        }
      }
    }

    return points;
  }

  /**
   * @brief Parse the StripSeeder spicific parameters from the PVL
   *
   * @param pvl The PVL object containing the control parameters for this
   * polygon seeder.
   */
  void StripPolygonSeeder::Parse(Pvl &pvl) {
    // Call the parents Parse method
    PolygonSeeder::Parse(pvl);

    // Pull parameters specific to this algorithm out
    try {
      // Get info from Algorithm group
      PvlGroup &algo = pvl.findGroup("PolygonSeederAlgorithm", Pvl::Traverse);
      PvlGroup &invalgo = invalidInput->findGroup("PolygonSeederAlgorithm",
                          Pvl::Traverse);

      // Set the spacing
      p_Xspacing = 0.0;
      if(algo.hasKeyword("XSpacing")) {
        p_Xspacing = (double) algo["XSpacing"];
        if(invalgo.hasKeyword("XSpacing")) {
          invalgo.deleteKeyword("XSpacing");
        }
      }
      else {
        QString msg = "PVL for StripSeeder must contain [XSpacing] in [";
        msg += pvl.fileName() + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      p_Yspacing = 0.0;
      if(algo.hasKeyword("YSpacing")) {
        p_Yspacing = (double) algo["YSpacing"];
        if(invalgo.hasKeyword("YSpacing")) {
          invalgo.deleteKeyword("YSpacing");
        }
      }
      else {
        QString msg = "PVL for StripSeeder must contain [YSpacing] in [";
        msg += pvl.fileName() + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    catch(IException &e) {
      QString msg = "Improper format for PolygonSeeder PVL [" + pvl.fileName() + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(p_Xspacing <= 0.0) {
      IString msg = "X Spacing must be greater that 0.0 [(" + IString(p_Xspacing) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    if(p_Yspacing <= 0.0) {
      IString msg = "Y Spacing must be greater that 0.0 [(" + IString(p_Yspacing) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  PvlGroup StripPolygonSeeder::PluginParameters(QString grpName) {
    PvlGroup pluginInfo(grpName);

    PvlKeyword name("Name", Algorithm());
    PvlKeyword minThickness("MinimumThickness", toString(MinimumThickness()));
    PvlKeyword minArea("MinimumArea", toString(MinimumArea()));
    PvlKeyword xSpac("XSpacing", toString(p_Xspacing));
    PvlKeyword ySpac("YSpacing", toString(p_Yspacing));

    pluginInfo.addKeyword(name);
    pluginInfo.addKeyword(minThickness);
    pluginInfo.addKeyword(minArea);
    pluginInfo.addKeyword(xSpac);
    pluginInfo.addKeyword(ySpac);

    return pluginInfo;
  }

}; // End of namespace Isis


/**
 * @brief Create a StripSeeder object
 *
 * Used to create a StripSeeder object from a PolygonSeeder plugin PVL
 * file.
 *
 * @param pvl The Pvl object that describes how the new object should be
 *           initialized.
 *
 * @return A pointer to the new object
 */
extern "C" Isis::PolygonSeeder *StripPolygonSeederPlugin(Isis::Pvl &pvl) {
  return new Isis::StripPolygonSeeder(pvl);
}

