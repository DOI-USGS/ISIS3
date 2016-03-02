/**                                                                       
 * @file                                                                  
 * $Revision: 6513 $
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $
 * $Id: GisGeometry.cpp 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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
#include "GisGeometry.h"

// Qt library
#include <QDebug>
#include <QScopedPointer>
#include <QString>

// geos library
#include <geos_c.h>

// other ISIS
#include "Cube.h"
#include "IException.h"
#include "GisBlob.h"
#include "GisTopology.h"
#include "SpecialPixel.h"

//#define DISABLE_PREPARED_GEOMETRY 1

using namespace std;

namespace Isis {

  /** 
   *  
   */  
  GisGeometry::GisGeometry() : m_type(None), m_geom(0), m_preparedGeom(0) { 
    // Must ensure GEOS is initialized
    GisTopology::instance();
  }
  

  /** 
   * @param xlongitude
   * @param ylatitude
   */  
  GisGeometry::GisGeometry(const double xlongitude, const double ylatitude) :
                           m_type(GeosGis), m_geom(0), m_preparedGeom(0){
  
    GisTopology::instance();
    m_geom = makePoint(xlongitude, ylatitude);
    m_preparedGeom = makePrepared(this->m_geom);
    return;
  }
  
  
  /** 
   * @param cube  
   */  
  GisGeometry::GisGeometry(Cube &cube) : m_type(IsisCube), m_geom(0), m_preparedGeom(0) {
  
    GisTopology::instance();
    m_geom = fromCube(cube);
    m_preparedGeom = makePrepared(this->m_geom);
    return;
  }
  

  /** 
   * @param gisSource The GIS Source. This might be a wkt string, a wkb string, 
   *                  or the file name of an ISIS cube.
   * @param t Type of GisGeometry to construct.
   * 
   */  
  GisGeometry::GisGeometry(const QString &gisSource, const GisGeometry::Type t) : 
                           m_type(t), m_geom(0), m_preparedGeom(0) {
    GisTopology *gis(GisTopology::instance());
    if (WKT == t) {
      m_geom = gis->geomFromWKT(gisSource);
    }
    else if (WKB == t) {
      m_geom = gis->geomFromWKB(gisSource);
    }
    else if (IsisCube == t) {
      Cube cube;
      cube.open(gisSource);
      m_geom = fromCube(cube);
    }
    else {
      throw IException(IException::Programmer, 
                       QString("Unknown GIS type given [%1]").arg(typeToString(t)), 
                       _FILEINFO_);
    }
  
    //  Get the prepared geometry
    m_preparedGeom = makePrepared(this->m_geom);
    m_type = t;
    return;
  }
  

  /** 
   * @param geom  
   */  
  GisGeometry::GisGeometry(const GisGeometry &geom) : m_type(geom.m_type),
                                                      m_geom(0),
                                                      m_preparedGeom(0) {
  
    GisTopology *gis(GisTopology::instance());
    if ( geom.isDefined() ) {
      m_geom = gis->clone(geom.m_geom);
      m_preparedGeom = makePrepared(this->m_geom);
    }
  }
  

  /** 
   * @param geom 
   */  
  GisGeometry::GisGeometry(GEOSGeometry *geom) : m_type(GeosGis), m_geom(geom),
                                                 m_preparedGeom(0) {
    GisTopology::instance();
    m_preparedGeom = makePrepared(this->m_geom);
    return;
  }
  

  /** 
   *  
   */  
  GisGeometry::~GisGeometry() {  
    destroy(); 
  }
  

  /** 
   * @param geom
   * @return GisGeometry
   */  
  GisGeometry &GisGeometry::operator=(GisGeometry const &geom) {
    if ( this != &geom ) {
      destroy();
      m_type = geom.m_type;
      if ( geom.isDefined() ) {
        GisTopology *gis(GisTopology::instance());
        m_geom = gis->clone(geom.m_geom);
        m_preparedGeom = makePrepared(this->m_geom);
      }
    }
    return (*this);
  }
  
  
  /**
   * @brief Set the geometry directly taking ownership 
   * 
   * @author 2012-07-14 Kris Becker
   * 
   * @param geom 
   */
  void GisGeometry::setGeometry(GEOSGeometry *geom) {
    destroy();
    m_geom = geom;
    m_preparedGeom = makePrepared(this->m_geom);
    return;
  }
  
  
  /** 
   * @return bool
   */  
  bool GisGeometry::isDefined() const {
    return (m_geom != 0);
  }
  

  /** 
   *  
   * @return bool
   */  
  bool GisGeometry::isValid() const {
    if (!isDefined()) {
      return (false);
    }
    
    return (1 == GEOSisValid(this->m_geom));
  }
  

  /** 
   *  
   * @return QString 
   */  
  QString GisGeometry::isValidReason() const {
    QString result = "Not defined!";
    if ( isDefined() ) {
      GisTopology *gis(GisTopology::instance());
      char *reason = GEOSisValidReason(this->m_geom);
      result = reason;
      gis->destroy(reason);
    }
    return (result);
  }
  

  /** 
   *  
   * @return GisGeometry::Type
   */  
  GisGeometry::Type GisGeometry::type() const {
    return (m_type);
  }
  

  /** 
   *  
   * @param gstrType
   * @return GisGeometry::Type
   */  
  GisGeometry::Type GisGeometry::type(const QString &gstrType) {
    QString gtype = gstrType.toLower();
    if ("wkt" == gtype) return (WKT);
    if ("wkb" == gtype) return (WKB);
    if ("cube" == gtype) return (IsisCube);
    if ("isiscube" == gtype) return (IsisCube);
    if ("geometry" == gtype) return (GeosGis);
    if ("geosgis" == gtype) return (GeosGis);
    if ("gis" == gtype) return (GeosGis);
    if ("geos" == gtype) return (GeosGis);
    return (None);
  }


  /** 
   *  
   * @param gstrType
   * @return GisGeometry::Type
   */  
  QString GisGeometry::typeToString(const GisGeometry::Type &type) {
    if (WKT == type) return "WKT";
    if (WKB == type) return "WKB";
    if (IsisCube == type) return "IsisCube";
    if (GeosGis == type) return "GeosGis";
    return "None";
  }
  

  /** 
   *  
   * @return GEOSGeometry
   */  
  const GEOSGeometry *GisGeometry::geometry() const {
    return (m_geom);
  }
  

  /** 
   *  
   * @return GEOSPreparedGeometry
   */  
  const GEOSPreparedGeometry *GisGeometry::preparedGeometry() const {
    return (m_preparedGeom);
  }
  
  
  /** 
   *  
   * @return GisGeometry
   */  
  GisGeometry *GisGeometry::clone() const {
    if (!isDefined()) {
      return (new GisGeometry());
    }
  
    GisTopology *gis(GisTopology::instance());
    QScopedPointer<GisGeometry> geom(new GisGeometry());
  
    geom->m_type = m_type;
    geom->m_geom = gis->clone(m_geom);
    geom->m_preparedGeom = makePrepared(geom->m_geom);
    return (geom.take());
  }
  
  
  /** 
   *  
   * @return bool
   */  
  bool GisGeometry::isEmpty() const {
    if ( !isValid() ) { 
      return (true); 
    }
    return (1 == GEOSisEmpty(this->m_geom));
  }
  

  /** 
   *  
   * @return double
   */  
  double GisGeometry::area( ) const {
    if ( !isValid() ) { 
      return (0.0); 
    }
  
    int result = 0;
    double gisArea = 0.0;
    result = GEOSArea(this->m_geom, &gisArea);
    if (1 != result) { 
      gisArea = 0.0; 
    }
    return (gisArea);
  }
  

  /** 
   *  
   * @return double
   */  
  double GisGeometry::length( ) const {
    if ( !isValid() ) { 
      return (0.0); 
    }
  
    int result = 0;
    double gisLength = 0.0;
    result = GEOSLength(this->m_geom, &gisLength);
    if (1 != result) { 
      gisLength = 0.0; 
    }
    return (gisLength);
  }
  

  /** 
   * @param target
   * @return double
   */  
  double GisGeometry::distance(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (false); 
    }
    if ( !target.isValid() ) { 
      return (false); 
    }
  
    int result = 0;
    double dist = Null;
    result = GEOSDistance(this->m_geom, target.geometry(), &dist);
    if ( 1 != result ) { 
      dist = Null; 
    }
    return (dist);
  }
 

  /** 
   * Get number of points in geometry 
   */
  int GisGeometry::points() const {
    if (!isValid() ) { return (0); }

    int ngeoms =  GEOSGetNumGeometries(m_geom);
    int npoints = 0;
    for (int i = 0 ; i < ngeoms ; i++) {
      npoints += GEOSGetNumCoordinates(GEOSGetGeometryN(m_geom, i));
    }

    return ( npoints );
  }


  /** 
   *  
   * @param target
   * @return bool
   */  
  bool GisGeometry::intersects(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (false);
    }
    if ( !target.isValid() ) { 
      return (false); 
    }
  
    int result = 0;
    if ( 0 != this->m_preparedGeom) {
      result = GEOSPreparedIntersects(this->m_preparedGeom, target.geometry()); 
    }
    else {
      result = GEOSIntersects(this->m_geom, target.geometry());
    }
    
    return (1 == result);
  }
  

  /** 
   * @param target
   * @return bool
   *  
   */  
  bool GisGeometry::contains(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (false); 
    }
    if ( !target.isValid() ) { 
      return (false);
    }
  
    int result = 0;
    if ( 0 != this->m_preparedGeom) {
      result = GEOSPreparedContains(this->m_preparedGeom, target.geometry());
    }
    else {
      result = GEOSContains(this->m_geom, target.geometry());
    }
    
    return (1 == result);
  }
  

  /** 
   *  
   * @param target
   * @return bool
   */  
  bool GisGeometry::disjoint(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (false); 
    }
    if ( !target.isValid() ) { 
      return (false); 
    }
  
    int result = 0;
    if ( 0 != m_preparedGeom) {
      result = GEOSPreparedDisjoint(m_preparedGeom, target.geometry());
    }
    else {
      result = GEOSDisjoint(m_geom, target.geometry());
    }
    
    return (1 == result);
  }
  

  /** 
   *  
   * @param target
   * @return bool
   */  
  bool GisGeometry::overlaps(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (false); 
    }
    if ( !target.isValid() ) { 
      return (false); 
    }
  
    int result = 0;
    if ( 0 != m_preparedGeom) {
      result = GEOSPreparedOverlaps(m_preparedGeom, target.geometry());
    }
    else {
      result = GEOSOverlaps(m_geom, target.geometry());
    }
    
    return (1 == result);
  }
  

  /** 
   *  
   * @param target
   * @return double
   */  
  double GisGeometry::intersectRatio(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (0.0); 
    }
    if ( !target.isValid() ) { 
      return (0.0); 
    }
  
  
    //  Check for any intersection at all
  //  if ( !intersects(target) ) {  
  //   return (0.0);
  // }

    // Prevent dividing by 0
    if (this->area() == 0) {
       return (0.0);
    }
  
    QScopedPointer<GisGeometry> inCommon(intersection(target));
    double ratio = inCommon->area() / this->area();
    return (ratio);
  }
  
  
  /** 
   * @return GisGeometry  
   */  
  GisGeometry *GisGeometry::envelope() const {
    if ( !isValid() ) { 
      return (new GisGeometry()); 
    }
  
    GEOSGeometry *geom = GEOSEnvelope(m_geom);
    return (new GisGeometry(geom));
  }
  

  /** 
   *  
   * @return GisGeometry
   */  
  GisGeometry *GisGeometry::convexHull() const {
    if ( !isValid() ) { 
      return (new GisGeometry()); 
    }
  
    GEOSGeometry *geom = GEOSConvexHull(m_geom);
    return (new GisGeometry(geom));
  }
  

 /**
  * @brief Simplify complex or overdetermined geoemtry 
  *  
  * This method will simplify a geometry with a Douglas -Peucker algorithm using 
  * a tolerance specifying the maximum distance from the original (multi)polygon. 
  * The use of this algorithm is designed to prevent oversimplification 
  * 
  * @author 2015-06-16 Kris Becker
  * 
  * @param tolerance Maximum distance from the original geometry expressed in the
  *                  coordinate system of the geometry.
  * 
  * @return GisGeometry* Simplified geometry
  */
  GisGeometry *GisGeometry::simplify(const double &tolerance) const {
    if ( !isValid() ) {
      return (0);
    }
    GEOSGeometry *geom = GEOSTopologyPreserveSimplify(m_geom, tolerance);
    if ( 0 == geom ) {
      return (0);
    }

    return (new GisGeometry(geom));
  }


  /** 
   *  
   * @param target
   * @return GisGeometry
   */  
  GisGeometry *GisGeometry::intersection(const GisGeometry &target) const {
    //  Non-valid geometries return empty geometries
    if ( !isValid() ) { 
      return (new GisGeometry()); 
    }
    if ( !target.isValid() ) { 
      return (new GisGeometry()); 
    }
  
    GEOSGeometry *geom = GEOSIntersection(m_geom, target.geometry());
    return (new GisGeometry(geom));
  }
  

  /** 
   *  
   * @param target
   * @return GisGeometry
   */  
  GisGeometry *GisGeometry::g_union(const GisGeometry &target) const {
    if ( !isValid() ) { 
      return (new GisGeometry()); 
    }
    if ( !target.isValid() ) { 
      return (new GisGeometry()); 
    }
  
    GEOSGeometry *geom = GEOSUnion(m_geom, target.geometry());
    return (new GisGeometry(geom));
  }
  

  /** 
   *  
   * @param xlongitude
   * @param ylatitude
   * @return bool
   */  
  bool GisGeometry::centroid(double &xlongitude, double &ylatitude) const {
    xlongitude = ylatitude = Null;
    if ( !isValid() ) { 
      return (false); 
    }
  
    GEOSGeometry *center = GEOSGetCentroid(m_geom);
    if ( 0 != center ) {
      GEOSGeomGetX(center, &xlongitude);
      GEOSGeomGetY(center, &ylatitude);
      GisTopology::instance()->destroy(center);
      return (true);
    }
  
    return (false);
  }
  

  /** 
   *  
   * @return GisGeometry
   */  
  GisGeometry *GisGeometry::centroid() const { 
    if ( !isValid() ) { 
      return (new GisGeometry()); 
    }
  
    GEOSGeometry *center = GEOSGetCentroid(m_geom);
    return (new GisGeometry(center));
  }
  

  /** 
   *  
   * @param geom
   * @return GEOSPreparedGeometry
   */  
  GEOSPreparedGeometry const *GisGeometry::makePrepared(const GEOSGeometry *geom) 
                                                        const {
  #if defined(DISABLE_PREPARED_GEOMETRY)
    return (0);
  #else
    GisTopology *gis(GisTopology::instance());
    return (gis->preparedGeometry(geom));
  #endif
  }
  

  /** 
   *  
   * @param x
   * @param y
   * @return GEOSGeometry
   */  
  GEOSGeometry *GisGeometry::makePoint(const double x, const double y) const {
  
    GEOSCoordSequence *point = GEOSCoordSeq_create(1, 2);
    GEOSCoordSeq_setX(point, 0, x);
    GEOSCoordSeq_setY(point, 0, y);
  
    return (GEOSGeom_createPoint(point));
  }
  

  /** 
   *  
   * @param cube
   * @return GEOSGeometry
   */  
  GEOSGeometry *GisGeometry::fromCube(Cube &cube) const {
    GisBlob myGis(cube);
    GisTopology *gis(GisTopology::instance());
    return (gis->geomFromWKT(myGis.polygon()));
  }
  

  /** 
   *  
   */  
  void GisGeometry::destroy() {
    GisTopology *gis(GisTopology::instance());
    gis->destroy(m_geom);
    gis->destroy(m_preparedGeom);
    m_geom = 0;
    m_preparedGeom = 0;
    return;
  }

}  //namespace Isis
