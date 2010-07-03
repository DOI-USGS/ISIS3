#ifndef Polygontools_h
#define Polygontools_h
/**
 * @file
 * $Revision: 1.23 $
 * $Date: 2009/07/17 16:13:46 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "geos/geom/GeometryFactory.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateSequence.h"

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
 * 
 */

  class UniversalGroundMap;
  class Projection;

  static geos::geom::GeometryFactory globalFactory;

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
 * 
 *   @history 2007-05-04 Robert Sucharski - Moved the method to
 *            output WKT from ImagePlygon class to this class.
 *            Also added method to output GML format.
 * 
 *   @history 2007-11-09 Tracie Sucharski - Remove ToWKT method, geos
 *             now has a method (toString) to return a WKT string.
 *             Added To180 method which converts polygon coordinates from
 *             0/360 system to -180/180 system.  If polygon was split because
 *             it crossed the 0/360 seam, the two polys coordinates are
 *             converted then merged.
 *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
 *   @history 2008-08-18 Steven Lambright - Updated to work with geos3.0.0
 *            instead of geos2. Mostly namespace changes.
 *   @history 2008-11-10 Christopher Austin - Added Thickness()
 *   @history 2008-11-25 Steven Koechle - Moved Despike Methods from
 *            ImageOverlapSet to PolygonTools
 *   @history 2008-12-01 Steven Lambright - Changed the Despike algorithm to be
 *            in more methods to clean it up, added the middle point to
 *            beginning/end of line tests to keep more data. Added "IsSpiked"
 *            and "TestSpiked."
 *   @history 2008-12-10 Steven Koechle - Moved MakeMultiPolygon Method from
 *             ImageOverlapSet to PolygonTools
 *   @history 2008-12-12 Steven Lambright - Bug fixes, cleaned up
 *            Despike/MakeMultiPolygon
 *   @history 2008-12-12 Steven Lambright - Renamed methods, polygon conversion
 *            methods now throw an iException if they fail, updated
 *            Despike(...)'s algorithm
 *   @history 2008-12-19 Steven Lambright - updated Despike(...)'s algorithm
 *   @history 2008-12-19 Steven Lambright - Added error to Despike (empty or
 *            invalid result).
 *   @history 2009-01-16 Steven Koechle - Fixed Memory Leak in
 *            LatLonToSampleLine method
 *   @history 2009-01-23 Steven Lambright - Added precision reduction algorithms
 *            and made the Difference and Intersect operators work in a more
 *            generic way by calling the new method Operate(...).
 *   @history 2009-01-28 Steven Lambright - Fixed memory leaks
 *   @history 2009-02-02 Stacy Alley - updated the To180 method
 *            to handle 360 multi polys that cross the -180/180
 *            boundry.  We need to return >1 polygon for these
 *            type of cases.
 *   @history 2009-01-28 Steven Lambright - Fixed bug in Operate(...) method
 *            when reducing precision
 *   @history 2009-06-09 Steven Lambright - Added a check to Equal(double,double). This
 *            never caused a problem but could have.
 */                                                                       

  class PolygonTools {

    public:
      PolygonTools ();
      ~PolygonTools ();

      static geos::geom::MultiPolygon *LatLonToXY(
          const geos::geom::MultiPolygon &lonLatPoly, Projection *proj);

      static geos::geom::MultiPolygon *XYToLatLon(
          const geos::geom::MultiPolygon &xYPoly, Projection *proj);

      static geos::geom::MultiPolygon *LatLonToSampleLine(
          const geos::geom::MultiPolygon &lonLatPoly, UniversalGroundMap *ugm);

      // Return a deep copy of a multpolygon
      static geos::geom::MultiPolygon* CopyMultiPolygon(const geos::geom::MultiPolygon *mpolygon);
      static geos::geom::MultiPolygon* CopyMultiPolygon(const geos::geom::MultiPolygon &mpolygon);

      static geos::geom::MultiPolygon* Despike (const geos::geom::Geometry *geom);
      static geos::geom::MultiPolygon* Despike (const geos::geom::MultiPolygon *multiPoly);
      static geos::geom::LinearRing* Despike (const geos::geom::LineString *linearRing);

      //  Return polygon in -180/180 coordinated system and merge split polys
      static geos::geom::MultiPolygon *To180 (geos::geom::MultiPolygon *poly360);

      //Return a polygon in GML format
      static std::string ToGML(const geos::geom::MultiPolygon *mpolygon, std::string idString="0");

      //Return the thickness of a polygon
      static double Thickness( const geos::geom::MultiPolygon *mpolygon );

      static geos::geom::Geometry *Intersect(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2);
      static geos::geom::Geometry *Difference(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2);

      static geos::geom::MultiPolygon* MakeMultiPolygon (const geos::geom::Geometry *geom);
      
      static std::string GetGeometryName(const geos::geom::Geometry *geom);

      static bool Equal( const geos::geom::MultiPolygon * poly1, const geos::geom::MultiPolygon * poly2 );
      static bool Equal( const geos::geom::Polygon * poly1, const geos::geom::Polygon * poly2 );
      static bool Equal( const geos::geom::LineString * lineString1, const geos::geom::LineString * lineString2 );
      static bool Equal( const geos::geom::Coordinate & coord1, const geos::geom::Coordinate & coord2 );
      static bool Equal( const double d1, const double d2 );
  private:
      geos::geom::MultiPolygon *p_polygons;

      //! Returns true if the middle point is spiked
      static bool IsSpiked(geos::geom::Coordinate first, geos::geom::Coordinate middle, geos::geom::Coordinate last); 
      //! Used by IsSpiked to directionally test (first/last matter) the spike 
      static bool TestSpiked(geos::geom::Coordinate first, geos::geom::Coordinate middle, geos::geom::Coordinate last); 

      static geos::geom::Geometry     *FixGeometry(const geos::geom::Geometry *geom);
      static geos::geom::MultiPolygon *FixGeometry(const geos::geom::MultiPolygon *poly);
      static geos::geom::Polygon      *FixGeometry(const geos::geom::Polygon *poly);
      static geos::geom::LinearRing   *FixGeometry(const geos::geom::LinearRing *ring);

  public:
      static geos::geom::Geometry     *ReducePrecision(const geos::geom::Geometry *geom, unsigned int precision);
      static geos::geom::MultiPolygon *ReducePrecision(const geos::geom::MultiPolygon *poly, unsigned int precision);
      static geos::geom::Polygon      *ReducePrecision(const geos::geom::Polygon *poly, unsigned int precision);
      static geos::geom::LinearRing   *ReducePrecision(const geos::geom::LinearRing *ring, unsigned int precision);
      static geos::geom::Coordinate   *ReducePrecision(const geos::geom::Coordinate *coord, unsigned int precision);
      static double ReducePrecision(double num, unsigned int precision);
  private:

      static geos::geom::Geometry *Operate(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2, unsigned int opcode);

      static int DecimalPlace(double);
  };
};

#endif

