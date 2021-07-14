#ifndef Polygontools_h
#define Polygontools_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/CoordinateSequence.h>

#include <QString>

namespace Isis {

  /**
   * @brief General tools for manipulating polygons
   *
   * Tools for manipulating polygons within Isis. These include
   * converting a Lon/Lat polygon to X/Y using a projection.
   *
   * @ingroup Utility
   *
   * @author 2006-06-22 Stuart Sides
   *
   * @internal
   *   @history 2008-08-18 Steven Lambright Updated to work with geos3.0.0
   *   @history 2009-02-05 Steven Lambright The FixGeometry methods will no longer
   *            produce geometries containing empty elements (though they might
   *            still return a completely empty geos figure). For example, a
   *            multipolygon will not have "EMPTY" polygons inside of it even
   *            though the multipolygon itself may be just EMPTY. This fixes
   *            issues with the geos BinaryOp(...) call.
   *   @history 2009-02-13 Steven Lambright Despike(geos::geom::MultiPolygon *) is
   *            now much more accepting of invalid polygons inside of the
   *            MultiPolygon. Often times perfectly good polygons are mixed with
   *            tiny, scattered, invalid polygons and this should now just throw
   *            those out and keep what it can.
   *   @history 2011-05-20 Steven Lambright To180 now catches all errors as it
   *            should and also works in more cases.
   *   @history 2017-08-30 Ian Humphrey - Modified the BinaryOp call to release the auto_ptr
   *                           geos returns and store it in a unique_ptr (clang c++11).
   *                           References #4809.
   */

  class UniversalGroundMap;
  class TProjection;
  
  
  static geos::geom::GeometryFactory::Ptr globalFactory = geos::geom::GeometryFactory::create();

  /**
   * @brief Provides various tools to work with geos multipolygons
   *
   * This class provides methods to that work with geos multipolygons. This
   * includes functions to convert from one coordinate system to another and to
   * copy multipolygons.
   *
   * @ingroup Utility
   *
   * @author 2006-08-07 Stuart Sides
   *
   * @internal
   *   @history 2006-08-07 Stuart Sides - Original version
   *   @history 2007-05-04 Robert Sucharski - Moved the method to
   *                           output WKT from ImagePlygon class to this class.
   *                           Also added method to output GML format.
   *   @history 2007-11-09 Tracie Sucharski - Remove ToWKT method, geos
   *                           now has a method (toString) to return a WKT string.
   *                           Added To180 method which converts polygon coordinates from
   *                           0/360 system to -180/180 system.  If polygon was split because
   *                           it crossed the 0/360 seam, the two polys coordinates are
   *                           converted then merged.
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *   @history 2008-08-18 Steven Lambright - Updated to work with geos3.0.0
   *                           instead of geos2. Mostly namespace changes.
   *   @history 2008-11-10 Christopher Austin - Added Thickness()
   *   @history 2008-11-25 Steven Koechle - Moved Despike Methods from
   *                           ImageOverlapSet to PolygonTools
   *   @history 2008-12-01 Steven Lambright - Changed the Despike algorithm to be
   *                           in more methods to clean it up, added the middle point to
   *                           beginning/end of line tests to keep more data. Added "IsSpiked"
   *                           and "TestSpiked."
   *   @history 2008-12-10 Steven Koechle - Moved MakeMultiPolygon Method from
   *                            ImageOverlapSet to PolygonTools
   *   @history 2008-12-12 Steven Lambright - Bug fixes, cleaned up
   *                           Despike/MakeMultiPolygon
   *   @history 2008-12-12 Steven Lambright - Renamed methods, polygon conversion
   *                           methods now throw an iException if they fail, updated
   *                           Despike(...)'s algorithm
   *   @history 2008-12-19 Steven Lambright - updated Despike(...)'s algorithm
   *   @history 2008-12-19 Steven Lambright - Added error to Despike (empty or
   *                           invalid result).
   *   @history 2009-01-16 Steven Koechle - Fixed Memory Leak in
   *                           LatLonToSampleLine method
   *   @history 2009-01-23 Steven Lambright - Added precision reduction algorithms
   *                           and made the Difference and Intersect operators work in a more
   *                           generic way by calling the new method Operate(...).
   *   @history 2009-01-28 Steven Lambright - Fixed memory leaks
   *   @history 2009-02-02 Stacy Alley - updated the To180 method
   *                           to handle 360 multi polys that cross the -180/180
   *                           boundry.  We need to return >1 polygon for these
   *                           type of cases.
   *   @history 2009-01-28 Steven Lambright - Fixed bug in Operate(...) method
   *                           when reducing precision
   *   @history 2009-06-09 Steven Lambright - Added a check to Equal(double,double). This
   *                           never caused a problem but could have.
   *   @history 2011-05-31 Steven Lambright - Improved To180 (not finished). The
   *                           remaining work is to remove the 0 seam from the polygons.
   *   @history 2013-02-26 Stuart Sides - Modified the output of GML and GML schema
   *   @history 2013-08-12 Stuart Sides - Added SplitPolygonOn360 and
   *                           FixPolePolygon methods.  Code was extracted from the ImagePolygon
   *                           class.  References #1604.
   *   @history 2017-08-18 Tyler Wilson, Summer Stapleton, Ian Humphrey - Changed auto_ptr references
   *                           to unique_ptr so that this class compiles with no warnings for 
   *                           C++14.  References #4809.
   */

  class PolygonTools {

    public:
      PolygonTools();
      ~PolygonTools();

      static geos::geom::MultiPolygon *LatLonToXY(
        const geos::geom::MultiPolygon &lonLatPoly, TProjection *proj);

      static geos::geom::MultiPolygon *XYToLatLon(
        const geos::geom::MultiPolygon &xYPoly, TProjection *proj);

      static geos::geom::MultiPolygon *LatLonToSampleLine(
        const geos::geom::MultiPolygon &lonLatPoly, UniversalGroundMap *ugm);

      // Return a deep copy of a multpolygon
      static geos::geom::MultiPolygon *CopyMultiPolygon(const geos::geom::MultiPolygon *mpolygon);
      static geos::geom::MultiPolygon *CopyMultiPolygon(const geos::geom::MultiPolygon &mpolygon);

      static geos::geom::MultiPolygon *Despike(const geos::geom::Geometry *geom);
      static geos::geom::MultiPolygon *Despike(const geos::geom::MultiPolygon *multiPoly);
      static geos::geom::LinearRing *Despike(const geos::geom::LineString *linearRing);

      //  Return polygon in -180/180 coordinated system and merge split polys
      static geos::geom::MultiPolygon *To180(geos::geom::MultiPolygon *poly360);

      //Return a polygon in GML format
      static QString ToGML(const geos::geom::MultiPolygon *mpolygon, 
                           QString idString = QString("0"), 
                           QString schema = QString(""));

      //Return the GML schema for a polygon
      static QString GMLSchema();

      //Return the thickness of a polygon
      static double Thickness(const geos::geom::MultiPolygon *mpolygon);

      static geos::geom::Geometry *Intersect(const geos::geom::Geometry *geom1,
                                             const geos::geom::Geometry *geom2);
      static geos::geom::Geometry *Difference(const geos::geom::Geometry *geom1,
                                              const geos::geom::Geometry *geom2);

      static geos::geom::MultiPolygon *MakeMultiPolygon(const geos::geom::Geometry *geom);

      static QString GetGeometryName(const geos::geom::Geometry *geom);

      static bool Equal(const geos::geom::MultiPolygon *poly1,
                        const geos::geom::MultiPolygon *poly2);
      static bool Equal(const geos::geom::Polygon *poly1, const geos::geom::Polygon *poly2);
      static bool Equal(const geos::geom::LineString *lineString1,
                        const geos::geom::LineString *lineString2);
      static bool Equal(const geos::geom::Coordinate &coord1, const geos::geom::Coordinate &coord2);
      static bool Equal(const double d1, const double d2);

      static geos::geom::MultiPolygon *FixSeam(const geos::geom::MultiPolygon *poly);
      static geos::geom::MultiPolygon *FixSeam(const geos::geom::Polygon *polyA,
                                               const geos::geom::Polygon *polyB);

      static geos::geom::Geometry     *ReducePrecision(const geos::geom::Geometry *geom,
                                                       unsigned int precision);
      static geos::geom::MultiPolygon *ReducePrecision(const geos::geom::MultiPolygon *poly,
                                                       unsigned int precision);
      static geos::geom::Polygon      *ReducePrecision(const geos::geom::Polygon *poly,
                                                       unsigned int precision);
      static geos::geom::LinearRing   *ReducePrecision(const geos::geom::LinearRing *ring,
                                                       unsigned int precision);
      static geos::geom::Coordinate   *ReducePrecision(const geos::geom::Coordinate *coord,
                                                       unsigned int precision);
      static double ReducePrecision(double num, unsigned int precision);

      static geos::geom::MultiPolygon *FixPolePolygon(const geos::geom::MultiPolygon *polePolygon,
                                                      UniversalGroundMap *ugm);
      static geos::geom::MultiPolygon *SplitPolygonOn360(const geos::geom::Polygon *inPoly);


    private:
      //! Returns true if the middle point is spiked
      static bool IsSpiked(geos::geom::Coordinate first,
                           geos::geom::Coordinate middle, geos::geom::Coordinate last);
      //! Used by IsSpiked to directionally test (first/last matter) the spike
      static bool TestSpiked(geos::geom::Coordinate first, geos::geom::Coordinate middle,
                             geos::geom::Coordinate last);

      static geos::geom::Geometry     *FixGeometry(const geos::geom::Geometry *geom);
      static geos::geom::MultiPolygon *FixGeometry(const geos::geom::MultiPolygon *poly);
      static geos::geom::Polygon      *FixGeometry(const geos::geom::Polygon *poly);
      static geos::geom::LinearRing   *FixGeometry(const geos::geom::LinearRing *ring);

      static geos::geom::Geometry *Operate(const geos::geom::Geometry *geom1,
                                           const geos::geom::Geometry *geom2, unsigned int opcode);

      static int DecimalPlace(double);

      geos::geom::MultiPolygon *p_polygons;

  };
};

#endif

