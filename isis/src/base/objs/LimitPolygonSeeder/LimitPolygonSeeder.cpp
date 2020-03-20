/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2010/05/05 21:24:30 $
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
#include <iomanip>

#include <geos/util/TopologyException.h>

#include "IException.h"
#include "LimitPolygonSeeder.h"
#include "PolygonTools.h"
#include "Pvl.h"
#include "PvlGroup.h"

namespace Isis {

  /**
   * @brief Construct a LimitPolygonSeeder algorithm
   *
   *
   * @param pvl  A Pvl object that contains a valid polygon point seeding
   *             definition
   */
  LimitPolygonSeeder::LimitPolygonSeeder(Pvl &pvl) : PolygonSeeder(pvl) {
    Parse(pvl);
  };


  /**
   * @brief Seed a polygon with points
   *
   * Seed the supplied polygon with points in a staggered pattern. The spacing
   * is determined by the PVL group "PolygonSeederAlgorithm"
   *
   * @param lonLatPoly geos::MultiPolygon The polygon to be seeded with points.
   *
   * @return std::vector<geos::Point*> A vector of points which have been
   * seeded into the polygon. The caller assumes responsibility for deleteing
   *  these.
   */
  std::vector<geos::geom::Point *> LimitPolygonSeeder::Seed(const geos::geom::MultiPolygon *multiPoly) {

    // Storage for the points to be returned
    std::vector<geos::geom::Point *> points;

    // Create some things we will need shortly
    const geos::geom::Envelope *polyBoundBox = multiPoly->getEnvelopeInternal();

    // Call the parents standardTests member
    QString msg = StandardTests(multiPoly, polyBoundBox);
    if(!msg.isEmpty()) {
      return points;
    }

    // Do limit seeder specific tests to make sure this poly should be seeded
    // (none for now)

    int xSteps = 0;
    int ySteps = 0;

    // Test if X is major axis
    if(fabs(polyBoundBox->getMaxX() - polyBoundBox->getMinX()) > fabs((polyBoundBox->getMaxY() - polyBoundBox->getMinY()))) {
      xSteps = p_majorAxisPts;
      ySteps = p_minorAxisPts;
    }
    else {
      xSteps = p_minorAxisPts;
      ySteps = p_majorAxisPts;
    }

    double xSpacing = (polyBoundBox->getMaxX() - polyBoundBox->getMinX()) / (xSteps);
    double ySpacing = (polyBoundBox->getMaxY() - polyBoundBox->getMinY()) / (ySteps);

    double realMinX = polyBoundBox->getMinX() + xSpacing / 2;
    double realMinY = polyBoundBox->getMinY() + ySpacing / 2;
    double maxY = polyBoundBox->getMaxY();
    double maxX = polyBoundBox->getMaxX();

    for(double y = realMinY; y < maxY; y += ySpacing) {
      for(double x = realMinX; x < maxX; x += xSpacing) {
        geos::geom::Geometry *gridSquarePolygon = GetMultiPolygon(x - xSpacing / 2.0, y - ySpacing / 2,
            x + xSpacing / 2.0, y + ySpacing / 2, *multiPoly);

        geos::geom::Point *centroid = gridSquarePolygon->getCentroid();

        delete gridSquarePolygon;
        if(centroid == NULL) continue;

        double gridCenterX = centroid->getX();
        double gridCenterY = centroid->getY();
        delete centroid;

        geos::geom::Coordinate c(gridCenterX, gridCenterY);
        points.push_back(Isis::globalFactory->createPoint(c));
      }
    }

    return points;
  }

  /**
   * This method returns the overlap between the x/y range specified and the
   * "orig" polygon. This is used to get polygons that represent the overlap
   * polygons in individual grid squares.
   *
   * @param dMinX Left side of rectangle
   * @param dMinY Bottom side of rectangle
   * @param dMaxX Right side of rectangle
   * @param dMaxY Top side of the rectangle
   * @param orig Multiplygon to intersect the square with
   *
   * @return geos::geom::Geometry* The portion of "orig" that intersects the
   *         square
   */
  geos::geom::Geometry *LimitPolygonSeeder::GetMultiPolygon(double dMinX, double dMinY,
      double dMaxX, double dMaxY,
      const geos::geom::MultiPolygon &orig) {
    geos::geom::CoordinateSequence *points = new geos::geom::CoordinateArraySequence();

    points->add(geos::geom::Coordinate(dMinX, dMinY));
    points->add(geos::geom::Coordinate(dMaxX, dMinY));
    points->add(geos::geom::Coordinate(dMaxX, dMaxY));
    points->add(geos::geom::Coordinate(dMinX, dMaxY));
    points->add(geos::geom::Coordinate(dMinX, dMinY));

    geos::geom::Polygon *poly = Isis::globalFactory->createPolygon(Isis::globalFactory->createLinearRing(points), NULL);
    geos::geom::Geometry *overlap = poly->intersection(&orig);

    return overlap;
  }

  /**
   * @brief Parse the LimitPolygonSeeder spicific parameters from the PVL
   *
   * @param pvl The PVL object containing the control parameters for this
   * polygon seeder.
   */
  void LimitPolygonSeeder::Parse(Pvl &pvl) {
    // Call the parents Parse method
    PolygonSeeder::Parse(pvl);

    // Pull parameters specific to this algorithm out
    try {
      // Get info from Algorithm group
      PvlGroup &algo = pvl.findGroup("PolygonSeederAlgorithm", Pvl::Traverse);
      PvlGroup &invalgo = invalidInput->findGroup("PolygonSeederAlgorithm",
                          Pvl::Traverse);

      // Set the spacing
      p_majorAxisPts = 0;
      if(algo.hasKeyword("MajorAxisPoints")) {
        p_majorAxisPts = (int) algo["MajorAxisPoints"];
        if(invalgo.hasKeyword("MajorAxisPoints")) {
          invalgo.deleteKeyword("MajorAxisPoints");
        }
      }
      else {
        QString msg = "PVL for LimitPolygonSeeder must contain [MajorAxisPoints] in [";
        msg += pvl.fileName() + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      p_minorAxisPts = 0;
      if(algo.hasKeyword("MinorAxisPoints")) {
        p_minorAxisPts = (int) algo["MinorAxisPoints"];
        if(invalgo.hasKeyword("MinorAxisPoints")) {
          invalgo.deleteKeyword("MinorAxisPoints");
        }
      }
      else {
        QString msg = "PVL for LimitPolygonSeeder must contain [MinorAxisPoints] in [";
        msg += pvl.fileName() + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    catch(IException &e) {
      QString msg = "Improper format for PolygonSeeder PVL [" + pvl.fileName() + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(p_majorAxisPts < 1.0) {
      IString msg = "Major axis points must be greater that 0.0 [(" + IString(p_majorAxisPts) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(p_minorAxisPts <= 0.0) {
      IString msg = "Minor axis points must be greater that 0.0 [(" + IString(p_minorAxisPts) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  PvlGroup LimitPolygonSeeder::PluginParameters(QString grpName) {
    PvlGroup pluginInfo(grpName);

    PvlKeyword name("Name", Algorithm());
    PvlKeyword minThickness("MinimumThickness", toString(MinimumThickness()));
    PvlKeyword minArea("MinimumArea", toString(MinimumArea()));
    PvlKeyword majAxis("MajorAxisPoints", toString(p_majorAxisPts));
    PvlKeyword minAxis("MinorAxisPoints", toString(p_minorAxisPts));

    pluginInfo.addKeyword(name);
    pluginInfo.addKeyword(minThickness);
    pluginInfo.addKeyword(minArea);
    pluginInfo.addKeyword(majAxis);
    pluginInfo.addKeyword(minAxis);

    return pluginInfo;
  }

}; // End of namespace Isis


/**
 * @brief Create a LimitPolygonSeeder object
 *
 * Used to create a LimitPolygonSeeder object from a PolygonSeeder plugin PVL
 * file.
 *
 * @param pvl The Pvl object that describes how the new object should be
 *           initialized.
 *
 * @return A pointer to the new object
 */
extern "C" Isis::PolygonSeeder *LimitPolygonSeederPlugin(Isis::Pvl &pvl) {
  return new Isis::LimitPolygonSeeder(pvl);
}

