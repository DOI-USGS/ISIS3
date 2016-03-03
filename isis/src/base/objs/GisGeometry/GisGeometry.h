#ifndef GisGeometry_h
#define GisGeometry_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: GisGeometry.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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

// GEOSGeometry, GEOSPreparedGeometry types
#include <geos_c.h>

// Qt library
#include <QMetaType>
#include <QSharedPointer>

class QString;

namespace Isis {

  class Cube;
  /**
   * @brief Encapsulation class provides support for GEOS-C API 
   *  
   * The Geometry Engine, Open Source (GEOS) software package, developed in C++ 
   * from a port of the Java Toplogy Suite (JTS) provides a simplied, generic C 
   * API using an opaque C pointer. This layer is to provide stable API from 
   * which to develop and maintain applications that are relatively immune from 
   * changes to the underlying C++ implementation. 
   *  
   * This class provides much of the basic elements to support simplified 
   * development of C++ applications with similiar goals - to provide immmunity 
   * from developmental changes to code using the GEOS-C API in a C++ 
   * development environment. 
   *  
   * The GEOS home page is http://trac.osgeo.org/geos/. The documentation for 
   * the GEOS-C package can be found at 
   * http://geos.osgeo.org/doxygen/c_iface.html. 
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2016-03-02 Ian Humphrey - Updated for coding standards compliance. Added to 
   *                           jwbacker's original unit test to prepare for adding this class to
   *                           ISIS. Fixes #2398.
   *   @history 2016-03-04 Kris Becker - Completed the documentation and implmented the equals()
   *                           method.
   */   
  class GisGeometry {
  
    public:
      /**
       * Source type of the geometry.
       */
      enum Type { 
          None,     //!< No geometry. A geometry object cannot be created with this geometry type.
          WKT,      //!< The GEOS library WKT reader is used to create the geometry.
          WKB,      //!< The GEOS library WKB reader is used to create the geometry.
          IsisCube, //!< An ISIS Cube is used to create the geometry.
          GeosGis   //!< GEOS GIS. A geometry object cannot be created with this geometry type. 
      };

      GisGeometry();
      GisGeometry(const double xlongitude, const double ylatitude);
      GisGeometry(Cube &cube);
      GisGeometry(const QString &gisSource, const Type t);
      GisGeometry(const GisGeometry &geom);
      GisGeometry(GEOSGeometry *geom);
      GisGeometry &operator=(GisGeometry const &geom);
      virtual ~GisGeometry();
  
      void setGeometry(GEOSGeometry *geom);
      
      bool isDefined() const;
      bool isValid() const;
      QString isValidReason() const;
      bool isEmpty() const;
      Type type() const;
      static Type type(const QString &gtype);
      static QString typeToString(const Type &type);
  
      const GEOSGeometry *geometry() const;
      const GEOSPreparedGeometry *preparedGeometry() const;
  
      GisGeometry *clone() const;
  
      // Useful operations
      double area() const;
      double length() const;
      double distance(const GisGeometry &target) const;
      int    points() const;
  
      bool intersects(const GisGeometry &target) const;
      bool contains(const GisGeometry &target) const;
      bool disjoint(const GisGeometry &target) const;
      bool overlaps(const GisGeometry &target) const;
      bool equals(const GisGeometry &target) const;
  
      double intersectRatio(const GisGeometry &geom) const;
  
      GisGeometry *envelope( ) const;
      GisGeometry *convexHull( ) const;
      GisGeometry *simplify(const double &tolerance) const;
  
      GisGeometry *intersection(const GisGeometry &geom) const;
      GisGeometry *g_union(const GisGeometry &geom) const;
  
      GisGeometry *centroid() const;
      bool centroid(double &xlongitude, double &ylatitude) const;
  
    private:
      GEOSGeometry *makePoint(const double x, const double y) const;
      GEOSGeometry *fromCube(Cube &cube) const;
      GEOSPreparedGeometry const *makePrepared(const GEOSGeometry *geom) const;
      void destroy();

      Type                 m_type;  //!< Geometry type of GIS source
      GEOSGeometry         *m_geom; //!< Pointer to GEOS-C opaque structure
      GEOSPreparedGeometry const *m_preparedGeom; //!< A prepared geometry from the GEOS library.
  
  };

  //! Definition for a SharedGisGeometry, a shared pointer to a GisGeometry 
  typedef QSharedPointer<GisGeometry> SharedGisGeometry;

} // Namespace Isis

// Declaration so this type can be used in Qt's QVariant class
Q_DECLARE_METATYPE(Isis::SharedGisGeometry);

#endif

