#ifndef ImagePolygon_h
#define ImagePolygon_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <sstream>
#include <vector>

#include "IException.h"
#include "Cube.h"
#include "Brick.h"
#include "Blob.h"
#include "Camera.h"
#include "Projection.h"
#include "UniversalGroundMap.h"
#include "Blob.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateArraySequence.h"

namespace Isis {

  /**
   * @brief Create cube polygons, read/write polygons to blobs
   *
   * This class creates polygons defining an image boundary, reads
   * the polygon from a cube blob, and writes a polygon to a cube
   * blob.  The GEOS (Geometry Engine Open Source) package is used
   * to create and manipulate the polygons. See
   * http://geos.refractions.net for information about this
   * package.
   *
   * @ingroup Registration
   *
   * @author 2005-11-22 Tracie Sucharski
   *
   * @internal
   *
   * @history 2006-05-17 Stuart Sides Renamed from PolygonTools and moved the
   *                                  geos::GeometryFactory to the new PolygonTools
   * @history 2007-01-04 Tracie Sucharski, Fixed bug in Fix360Poly, on
   *                non-pole images after splitting at the 360 boundary,
   *                wasn't insuring that start and end points of polygon were
   *                the same.
   * @history 2007-01-05 Tracie Sucharski, Made a change to the SetImage
   *                method.  Cannot make the assumption that if a sample/line
   *                returns a lat/lon, that the lat/lon will return a sample/line.
   *                This was found using a Moc WA, global image, sample 1, line 1
   *                returned a lat/lon, but entering that lat/lon does not return a
   *                valid sample/line.
   * @history 2007-01-08 Tracie Sucharski, Fixed bugs in the ToImage and
   *                ToGround class for images that contain a pole.
   *
   * @history 2007-01-19  Tracie Sucharski,  Comment out the ToGround method.
   *
   * @history 2007-01-31  Tracie Sucharski,  Add WKT method to return WKT polygon
   *                            as a string.
   * @history 2007-02-06  Tracie Sucharski,  Added band parameter to Create
   *                            method and call UniversalGroundMap::SetBand.
   * @history 2007-05-04  Robert Sucharski, with Jeff, Stuart, and
   *                               Tracie's blessing, moved the WKT method to the
   *                               PolygonTools class.
   * @history 2007-07-26  Tracie Sucharski,  Added ss,sl,ns,nl to constructor
   *                            for sub-poly creation.
   * @history 2007-11-09  Tracie Sucharski,  Remove WKT method since the geos
   *                            package know has a method to return a WKT
   *                            string
   * @history 2008-03-17  Tracie Sucharski, Added try/catch block to Create
   *                            and Fix360Poly to catch geos exceptions.
   *
   *  @history 2008-06-18 Stuart Sides - Fixed doc error
   *  @history 2008-08-18 Steven Lambright - Updated to work with geos3.0.0
   *           instead of geos2. Mostly namespace changes.
   *  @history 2008-12-10 Steven Koechle - Split pole code into seperate function,
   *           chanded the way FixPoly360 did its intersection to handle a
   *           multipolygon being returned.
   *  @history 2008-12-10 Steven Lambright - Fixed logical problem which could
   *           happen when the limb of the planet was on the left and was the
   *           first thing found in the FindPoly method.
   *  @history 2009-01-06 Steven Koechle - Changed Constructor, removed backwards
   *           compatibility for footprint name.
   *  @history 2009-01-28 Steven Lambright - Fixed memory leaks
   *  @history 2009-02-20 Steven Koechle - Fixed Fix360Poly to use the
   *           PolygonTools Intersect method.
   *  @history 2009-04-17 Steven Koechle - Rewrote most of the class. Implemented
   *           a left-hand rule AI walking algotithm in the walkpoly method. Pole
   *           and 360 boundary logic was more integrated and simplified. Method
   *           of finding the first point was seperated into its own method.
   *  @history 2009-05-06 Steven Koechle - Fixed Error where a NULL polygon was
   *           being written.
   *  @history 2009-05-28 Steven Lambright - This program will no longer generate
   *           invalid polygons and should use despike as a correction algorithm
   *           if it is necessary.
   *  @history 2009-05-28 Stuart Sides - Added a new argument to Create for
   *           controling the quality of the polygon.
   *  @history 2009-05-28 Steven Lambright - Moved the quality parameter in the
   *           argument list, improved speed and the edges of files (and subareas)
   *           should be checked if the algorithm reaches them now. Fixed the
   *           sub-area capabilities.
   *  @history 2009-06-16 Christopher Austin - Fixed WalkPoly() to catch large
   *           pixel increments that ( though snapping ) skip over the starting
   *           point. Also skip the first left turn in FindNextPoint() to prevent
   *           double point checking as well as errors with snapping.
   *  @history 2009-06-17 Christopher Austin - Removed p_name. Uses parent Name()
   *  @history 2009-06-18 Christopher Austin - Changes starting point skipping
   *           solution to a snap. Added fix for polygons that form
   *           self-intersecting polygons due to this first point snapping.
   *  @history 2009-06-19 Christopher Austin - Temporarily reverted for backport,
   *           fixed truthdata for SpiceRotation, re-commited with all updates
   *  @history 2009-07-01 Christopher Austin - Added Emission and Phase Angle
   *           tolerences.
   *  @history 2009-07-09 Christopher Austin - Fixed range error in FixPolePoly()
   *           when no points cross, an overflow exception, as well as emission
   *           and incidence handling of poles.
   *  @history 2009-07-21 Christopher Austin - Added limb snapping along with
   *           internal corner searching. Fixed lat/lon crossing when samp/line
   *           does not cross. Removed the SetUniversalGround call in SetImage().
   *           Limbs are now detected and handled using an ellipsoid shape model
   *           if p_ellipsoid is true.
   *  @history 2009-07-28 Christopher Austin - Changed qualitiy increment to
   *           sample/line increment to remove unnecessary points from the
   *           polygon.
   *  @history 2009-08-05 Christopher Austin - Added pole meridian proper polygon
   *           division.
   *  @history 2009-08-20 Christopher Austin - Added meridian walking and subpixel
   *           accuracy.
   *  @history 2009-08-28 Christopher Austin - Fixed a memory bounds error.
   *  @history 2010-02-08 Christopher Austin - Fixed an infinite loop which
   *           revisited the starting point, and added 360 meridian crossing
   *  @history 2010-02-17 Christopher Austin - Fixed two more infinite looping
   *           issues, including a cycle fix which occured during Emission Angle
   *           and Incidence Angle restrictions
   *  @history 2011-05-11 Steven Lambright - Now works with projected images.
   *  @history 2011-05-12 Christopher Austin - Added validSample/LineDim()
   *                                           functionality.
   *  @history 2011-07-01 Travis Addair - Added outlier check to
   *           FindFirstPoint() to avoid choosing a first point without any
   *           valid neighbors (i.e., an outlier).
   *  @history 2012-05-14 Steven Lambright - Added safety to the calcImageBorderCoordinates method,
   *                          along with the validSampleDim and validLineDim methods. This fixes
   *                          seg faults. Fixes #686.
   *  @history 2012-07-16 Steven Lambright - Fixed a bug in WalkPoly() that caused crashing
   *                          periodically due to accessing a vector outside of it's bounds
   *                          (negative indices). This was in the 'triangle' (loop) detection code.
   *                          Fixes #994.
   */

  class ImagePolygon {

    public:
      ImagePolygon();
      ImagePolygon(Blob &blob);
      ~ImagePolygon();

      void Create(Cube &cube, int sinc = 1, int linc = 1,
          int ss = 1, int sl = 1, int ns = 0, int nl = 0, int band = 1,
          bool increasePrecision = false);

      void Create(std::vector<std::vector<double>> polyCoordinates);

      Camera * initCube(Cube &cube, int ss = 1, int sl = 1,
                        int ns = 0, int nl = 0, int band = 1);

      /**
       * Set the maximum emission angle ( light refleted to camera )
       *
       * @param emission The maximum valid emission angle
       */
      void Emission(double emission) {
        p_emission = emission;
      }
      /**
       * Set the maximum incidence angle ( light contacting the planet )
       *
       * @param incidence The maximum valid incidence angle
       */
      void Incidence(double incidence) {
        p_incidence = incidence;
      }

      /**
       * If a limb is detected, use un ellipsoid shape model if true. If false,
       * use the default (spiceinit defined) shape model.
       *
       * @param ellip True to use ellipsoid on limb images
       */
      void EllipsoidLimb(bool ellip) {
        p_ellipsoid = ellip;
      }

      /**
       * The subpixel accuracy to use. This accuracy is the number of binary steps to
       * take to find the subpixel accuracy. A higher number provided gives more
       * accurate results at the cost of runtime.
       *
       * ImagePolygon's constructor sets a default value of 50
       *
       * @param div The subpixel accuracy to use
       */
      void SubpixelAccuracy(int div) {
        p_subpixelAccuracy = div;
      }

      //!  Return a geos Multipolygon
      geos::geom::MultiPolygon *Polys() {
        return p_polygons;
      };

      //!  Return a geos Multipolygon
      std::string polyStr() const {
        return p_polyStr;
      };

      double validSampleDim();
      double validLineDim();

      //!  Return the sample increment used the create this polygon
      int getSinc() const {
        return p_sampinc;
      }

      //!  Return the line increment used the create this polygon
      int getLinc() const {
        return p_lineinc;
      }

      int numVertices() const {
        return p_pts->getSize();
      }

      Blob toBlob() const;

    private:
      // Please do not add new polygon manipulation methods to this class.
      // Polygon manipulation should be done in the PolygonTools class.
      bool SetImage(const double sample, const double line);

      geos::geom::Coordinate FindFirstPoint();
      void WalkPoly();
      geos::geom::Coordinate FindNextPoint(geos::geom::Coordinate *currentPoint,
                                           geos::geom::Coordinate lastPoint,
                                           int recursionDepth = 0);

      double DistanceSquared(const geos::geom::Coordinate *p1, const geos::geom::Coordinate *p2);

      void MoveBackInsideImage(double &sample, double &line, double sinc, double linc);
      bool InsideImage(double sample, double line);
      void Fix360Poly();
      void FixPolePoly(std::vector<geos::geom::Coordinate> *crossingPoints);

      bool IsLimb();
      geos::geom::Coordinate FindBestPoint(geos::geom::Coordinate *currentPoint,
                                           geos::geom::Coordinate newPoint,
                                           geos::geom::Coordinate lastPoint);
      geos::geom::Coordinate FixCornerSkip(geos::geom::Coordinate *currentPoint,
                                           geos::geom::Coordinate newPoint);

      void FindSubpixel(std::vector<geos::geom::Coordinate> & points);

      void calcImageBorderCoordinates();

      Cube *p_cube;       //!< The cube provided
      bool p_isProjected; //!< True when the provided cube is projected

      Brick *p_brick;     //!< Used to check for valid DNs

      geos::geom::CoordinateArraySequence *p_pts; //!< The sequence of coordinates that compose the boundary of the image

      geos::geom::MultiPolygon *p_polygons;  //!< The multipolygon of the image

      std::string p_polyStr; //!< The string representation of the polygon

      UniversalGroundMap *p_gMap; //!< The cube's ground map

      geos::geom::Coordinate *m_leftCoord; //!< The cube's left-most valid coord
      geos::geom::Coordinate *m_rightCoord; //!< The cube's right-most valid coord
      geos::geom::Coordinate *m_topCoord; //!< The cube's top-most valid coord
      geos::geom::Coordinate *m_botCoord; //!< The cube's bot-most valid coord

      int p_cubeStartSamp; //!< The the sample of the first valid point in the cube
      int p_cubeStartLine; //!< The the line of the first valid point in the cube
      int p_cubeSamps;     //!< The number of samples in the cube
      int p_cubeLines;     //!< The number of lines in the cube

      int p_sampinc; //!< The increment for walking along the polygon in the sample direction
      int p_lineinc; //!< The increment for walking along the polygon in the line direcction

      double p_emission;  //!< The maximum emission angle to consider valid
      double p_incidence; //!< The maximum incidence angle to consider valid
      bool p_ellipsoid;   //!< Uses an ellipsoid if a limb is detected

      int p_subpixelAccuracy; //!< The subpixel accuracy to use

  };
};

#endif
