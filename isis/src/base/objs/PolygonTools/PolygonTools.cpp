/**                                                                       
 * @file                                                                  
 * $Revision: 1.26 $                                                             
 * $Date: 2010/02/24 01:11:52 $                                                                 
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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "geos/geom/BinaryOp.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "geos/operation/distance/DistanceOp.h"
#include "geos/opOverlay.h"
#include "geos/operation/overlay/snap/GeometrySnapper.h"

#include "SpecialPixel.h"
#include "PolygonTools.h"
#include "ProjectionFactory.h"
#include "UniversalGroundMap.h"

using namespace std;
namespace Isis {
  /**                                                                       
   * This method will return a geos::geom::MultiPolygon which contains the X/Y
   * coordinates of the LonLat polygon. The Lat/Lon polygon must 
   * have coordinates (x direction, y direction) or (Lon,Lat). 
   * 
   * @param lonLatPolygon  A multipolygon in (Lon,Lat) order
   * @param projection     The projection to be used to convert the Lons and Lat
   *                       to X and Y
   * 
   * @return  Returns a multipolygon which is the result of converting the input
   *          multipolygon from (Lon,Lat) to (X,Y).
   */
  geos::geom::MultiPolygon *PolygonTools::LatLonToXY(
      const geos::geom::MultiPolygon &lonLatPolygon, Projection *projection) {
    if (projection == NULL) {
      string msg = "Unable to convert Lon/Lat polygon to X/Y. ";
      msg += "No projection has was supplied";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    // Convert the Lat/Lon poly coordinates to X/Y coordinates
    if (lonLatPolygon.isEmpty()) {
      return globalFactory.createMultiPolygon();
    }
    else {
      vector<geos::geom::Geometry*> *xyPolys = new vector<geos::geom::Geometry*>;
      // Convert each polygon in this multi-polygon
      for (unsigned int g=0; g<lonLatPolygon.getNumGeometries(); ++g) {
        geos::geom::Polygon *poly = (geos::geom::Polygon*)(lonLatPolygon.getGeometryN(g));

        // Convert each hole inside this polygon
        vector<geos::geom::Geometry *> *holes = new vector<geos::geom::Geometry *>;
        for (unsigned int h=0; h<poly->getNumInteriorRing(); ++h) {
          geos::geom::CoordinateSequence *xycoords = new geos::geom::CoordinateArraySequence ();
          geos::geom::CoordinateSequence *llcoords =
              poly->getInteriorRingN(h)->getCoordinates();

          // Convert each coordinate in this hole
          for (unsigned int cord=0; cord < llcoords->getSize(); ++cord) {
            projection->SetGround(llcoords->getAt(cord).y,
                                    llcoords->getAt(cord).x);
            xycoords->add(geos::geom::Coordinate(projection->XCoord(),
                                           projection->YCoord()));
          } // end num coords in hole loop

          geos::geom::LinearRing *hole = globalFactory.createLinearRing(xycoords);

          if(hole->isValid() && !hole->isEmpty()) {
            holes->push_back(hole);
          }
          else {
            delete hole;
          }
        } // end num holes in polygon loop

        // Convert the exterior ring of this polygon
        geos::geom::CoordinateSequence *xycoords = new geos::geom::CoordinateArraySequence ();
        geos::geom::CoordinateSequence *llcoords =
            poly->getExteriorRing()->getCoordinates();

        // Convert each coordinate in the exterior ring of this polygon
        for (unsigned int cord=0; cord < llcoords->getSize(); ++cord) {
          projection->SetGround(llcoords->getAt(cord).y,
                                  llcoords->getAt(cord).x);
          xycoords->add(geos::geom::Coordinate(projection->XCoord(),
                                         projection->YCoord()));
        } // end exterior ring coordinate loop

        geos::geom::Polygon *newPoly = globalFactory.createPolygon(
            globalFactory.createLinearRing(xycoords), holes);

        if(newPoly->isValid() && !newPoly->isEmpty() && newPoly->getArea() > 1.0e-14) {
          xyPolys->push_back(newPoly);
        }
        else {
          delete newPoly;
        }
      } // end num geometry in multi-poly

      // Create a new multipoly from all the new X/Y polygon(s)
      geos::geom::MultiPolygon *spikedPoly = globalFactory.createMultiPolygon(xyPolys);

      if(spikedPoly->isValid() && !spikedPoly->isEmpty()) {
        return spikedPoly;
      }
      else {
        try {
          geos::geom::MultiPolygon *despikedPoly = Despike(spikedPoly);

          delete spikedPoly;
          spikedPoly = NULL;
  
          return despikedPoly;
        }
        catch (iException &e) {
          iString msg = "Unable to convert polygon from Lat/Lon to X/Y";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }

    } // end else
  }


  /**                                                                       
   * This method will return a geos::geom::MultiPolygon which contains the 
   * (Lon,Lat) coordinates of the XY polygon. The Lat/Lon polygon will 
   * have coordinates (x direction, y direction) or (Lon,Lat). 
   *
   * @param xYPolygon  A multipolygon in (X,Y) order
   * @param projection The projection to be used to convert the Xs and Ys to Lon
   *                   and Lats
   * 
   * @return  Returns a multipolygon which is the result of converting the input
   *          multipolygon from (X,Y) to (Lon,Lat).
   */
  geos::geom::MultiPolygon *PolygonTools::XYToLatLon(
      const geos::geom::MultiPolygon &xYPolygon, Projection *projection) {

    if (projection == NULL) {
      string msg = "Unable to convert X/Y polygon to Lon/Lat. ";
      msg += "No projection was supplied";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    // Convert the X/Y poly coordinates to Lat/Lon coordinates
    if (xYPolygon.isEmpty()) {
      return globalFactory.createMultiPolygon();
    }
    else {
      vector<geos::geom::Geometry*> *llPolys = new vector<geos::geom::Geometry*>;
      // Convert each polygon in this multi-polygon
      for (unsigned int g=0; g<xYPolygon.getNumGeometries(); ++g) {
        geos::geom::Polygon *poly = (geos::geom::Polygon*)(xYPolygon.getGeometryN(g));

        // Convert each hole inside this polygon
        vector<geos::geom::Geometry *> *holes = new vector<geos::geom::Geometry *>;
        for (unsigned int h=0; h<poly->getNumInteriorRing(); ++h) {
          geos::geom::CoordinateSequence *llcoords = new geos::geom::CoordinateArraySequence ();
          geos::geom::CoordinateSequence *xycoords =
              poly->getInteriorRingN(h)->getCoordinates();

          // Convert each coordinate in this hole
          for (unsigned int cord=0; cord < xycoords->getSize(); ++cord) {
            projection->SetWorld(xycoords->getAt(cord).x,
                                   xycoords->getAt(cord).y);
            llcoords->add(geos::geom::Coordinate(projection->Longitude(),
                                           projection->Latitude()));
          } // end num coords in hole loop
          holes->push_back(globalFactory.createLinearRing(llcoords));
        } // end num holes in polygon loop

        // Convert the exterior ring of this polygon
        geos::geom::CoordinateSequence *llcoords = new geos::geom::DefaultCoordinateSequence ();
        geos::geom::CoordinateSequence *xycoords =
            poly->getExteriorRing()->getCoordinates();

        // Convert each coordinate in the exterior ring of this polygon
        for (unsigned int cord=0; cord < xycoords->getSize(); ++cord) {
          projection->SetWorld(xycoords->getAt(cord).x,
                                 xycoords->getAt(cord).y);
          llcoords->add(geos::geom::Coordinate(projection->Longitude(),
                                         projection->Latitude()));
        } // end exterior ring coordinate loop

        llPolys->push_back(globalFactory.createPolygon(
            globalFactory.createLinearRing(llcoords), holes));
      } // end num geometry in multi-poly


      // Create a new multipoly from all the new Lat/Lon polygon(s)
      geos::geom::MultiPolygon *spikedPoly = globalFactory.createMultiPolygon(llPolys);

      if(spikedPoly->isValid() && !spikedPoly->isEmpty()) {
        return spikedPoly;
      }
      else {
        try {
          geos::geom::MultiPolygon *despikedPoly = Despike(spikedPoly);

          delete spikedPoly;
          spikedPoly = NULL;

          return despikedPoly;
        }
        catch (iException &e) {
          iString msg = "Unable to convert polygon from X/Y to Lat/Lon";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
    } // end else
  }


  /**                                                                       
   * This method will return a geos::geom::MultiPolygon which contains the 
   * sample/line coordinates of the Lat/Lon polygon. The Lat/Lon polygon must 
   * have coordinates (x direction, y direction) or (Lon,Lat). 
   *
   * @param lonLatPolygon  A multipolygon in (Lon,Lat order)
   * @param ugm            The UniversalGroundMap to be used to convert the Lons
   *                       and Lat to Samples and Lines
   * 
   * @return  Returns a multipolygon which is the result of converting the input
   *          multipolygon from (Lon,Lat) to (Sample,Line).
   */
  geos::geom::MultiPolygon *PolygonTools::LatLonToSampleLine(
      const geos::geom::MultiPolygon &lonLatPolygon, UniversalGroundMap *ugm) {

    if (ugm == NULL) {
      string msg = "Unable to convert Lon/Lat polygon to Sample/Line. ";
      msg += "No UniversalGroundMap was supplied";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  
    // Convert the Lon/Lat poly coordinates to Sample/Line coordinates
    if (lonLatPolygon.isEmpty()) {
      return globalFactory.createMultiPolygon();
    }
    else {
      vector<geos::geom::Geometry*> *slPolys = new vector<geos::geom::Geometry*>;
      // Convert each polygon in this multi-polygon
      for (unsigned int g = 0; g < lonLatPolygon.getNumGeometries(); g++) {
        geos::geom::Polygon *poly = (geos::geom::Polygon*)(lonLatPolygon.getGeometryN(g));
  
        // Convert each hole inside this polygon
        vector<geos::geom::Geometry *> *holes = new vector<geos::geom::Geometry *>;
        for (unsigned int h=0; h<poly->getNumInteriorRing(); ++h) {
          geos::geom::CoordinateSequence *slcoords = new geos::geom::DefaultCoordinateSequence ();
          geos::geom::CoordinateSequence *llcoords =
              poly->getInteriorRingN(h)->getCoordinates();
  
          // Convert each coordinate in this hole
          for (unsigned int cord=0; cord < llcoords->getSize(); ++cord) {
            ugm->SetUniversalGround(llcoords->getAt(cord).y,
                                                     llcoords->getAt(cord).x);
            slcoords->add(geos::geom::Coordinate(ugm->Sample(),
                                                 ugm->Line()));
          } // end num coords in hole loop
          holes->push_back(globalFactory.createLinearRing(slcoords));
          delete slcoords;
          delete llcoords;
        } // end num holes in polygon loop
  
        // Convert the exterior ring of this polygon
        geos::geom::CoordinateSequence *slcoords = new geos::geom::CoordinateArraySequence ();
        geos::geom::CoordinateSequence *llcoords =
            poly->getExteriorRing()->getCoordinates();
  
        // Convert each coordinate in the exterior ring of this polygon
        for (unsigned int cord=0; cord < llcoords->getSize(); ++cord) {
          ugm->SetUniversalGround(llcoords->getAt(cord).y,
                                                   llcoords->getAt(cord).x);
          slcoords->add(geos::geom::Coordinate(ugm->Sample(),
                                               ugm->Line()));
        } // end exterior ring coordinate loop
  
        slPolys->push_back(globalFactory.createPolygon(
            globalFactory.createLinearRing(slcoords), holes));
        delete llcoords;
      } // end num geometry in multi-poly
  
      // Create a new multipoly from all the new Sample/Line polygon(s)
      geos::geom::MultiPolygon *spikedPoly = globalFactory.createMultiPolygon(slPolys);

      if(spikedPoly->isValid() && !spikedPoly->isEmpty()) {
        return spikedPoly;
      }
      else {
        try {
          geos::geom::MultiPolygon *despikedPoly = Despike(spikedPoly);

          delete spikedPoly;
          spikedPoly = NULL;

          return despikedPoly;
        }
        catch (iException &e) {
          iString msg = "Unable to convert polygon from Lat/Lon to Sample/Line";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
    } // end else
  }


  /**                                                                       
   * This static method will create a deep copy of a geos::geom::MultiPolygon. The
   * caller assumes responsibility for the memory associated with the new
   * polygon.
   * 
   * @param mpolygon The multipolygon to be copied.
   * 
   * @return  Returns a pointer to a multipolygon which is a deep copy of the
   *          input multipolygon. This is necessary because at the time of
   *          writing the geos package does not create multipolygons when
   *          copying. It produdes geometryCollections
   */
  geos::geom::MultiPolygon* PolygonTools::CopyMultiPolygon(const geos::geom::MultiPolygon *mpolygon) {

    vector<geos::geom::Geometry*> *polys = new vector<geos::geom::Geometry*>;
    for (unsigned int i=0; i<mpolygon->getNumGeometries(); ++i) {
      polys->push_back((mpolygon->getGeometryN(i))->clone());
    }
    return globalFactory.createMultiPolygon (polys);
  }


  /**                                                                       
   * This static method will create a deep copy of a geos::geom::MultiPolygon. The
   * caller assumes responsibility for the memory associated with the new
   * polygon.
   * 
   * @param mpolygon The multipolygon to be copied.
   * 
   * @return  Returns a pointer to a multipolygon which is a deep copy of the
   *          input multipolygon. This is necessary because at the time of
   *          writing the geos package does not create multipolygons when
   *          copying. It produdes geometryCollections
   */
  geos::geom::MultiPolygon* PolygonTools::CopyMultiPolygon(const geos::geom::MultiPolygon &mpolygon) {

    vector<geos::geom::Geometry*> *polys = new vector<geos::geom::Geometry*>;
    for (unsigned int i=0; i<mpolygon.getNumGeometries(); ++i) {
      polys->push_back((mpolygon.getGeometryN(i))->clone());
    }
    return globalFactory.createMultiPolygon (polys);
  }


  /**
   * Write out the polygon with gml header
   * 
   * 
   * @param [in] mpolygon Polygon with lat/lon vertices
   * @param idString mpolygon's Id
   * @return istrean  Returns the polygon with lon,lat
   *         lon,lat format vertices and GML header
   */

  string PolygonTools::ToGML (const geos::geom::MultiPolygon *mpolygon, string idString) { 

    ostringstream os;

    //Write out the GML header  
    os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
    os << "<ogr:FeatureCollection" << endl;
    os << "    xmlns:xsi=\"http://www.w3c.org/2001/XMLSchema-instance\""<< endl;
    os << "    xmlns:gml=\"http://www.opengis.net/gml\">" << endl;
    os << "  <gml:boundedBy>" << endl;
    os << "    <gml:Box>" << endl;
    os << "      <gml:coord><gml:X>0.0</gml:X><gml:Y>-90.0</gml:Y></gml:coord>" << endl;
    os << "      <gml:coord><gml:X>360.0</gml:X><gml:Y>90.0</gml:Y></gml:coord>" << endl;
    os << "    </gml:Box>" << endl;
    os << "  </gml:boundedBy>" << endl;
    os << "  <gml:featureMember>" << endl;
    os << "   <multi_polygon fid=\"0\">" << endl;
    os << "      <ID>"<< idString << "</ID>" << endl;
    os << "      <ogr:geometryProperty><gml:MultiPolygon><gml:polygonMember>" <<
                 "<gml:Polygon><gml:outerBoundaryIs><gml:LinearRing><gml:coordinates>";


    for (unsigned int polyN=0; polyN<mpolygon->getNumGeometries(); polyN++) {
      geos::geom::CoordinateSequence *pts = mpolygon->getGeometryN(polyN)->getCoordinates();

      for (unsigned int i=0; i<pts->getSize(); i++) {
        double lon = pts->getAt(i).x;
        double lat = pts->getAt(i).y;

        os << setprecision(15) << lon << ","<< setprecision(15) << lat << " "; 
      }
    }

     os <<"</gml:coordinates></gml:LinearRing></gml:outerBoundaryIs>"<<
     "</gml:Polygon></gml:polygonMember></gml:MultiPolygon>"<<
     "</ogr:geometryProperty>" << endl; 
     os << "</multi_polygon>" << endl;
     os << "</gml:featureMember>" << endl;
     os << "</ogr:FeatureCollection>" << endl;

    return os.str();
  }


  /**
   * Convert polygon coordinates from 360 system to 180.  If polygon is split
   * into 2 polygons due to crossing the 360 boundary, convert the polygon
   * less than 360 to 180, then union together to merge the polygons. 
   * If the polygon was one poly, but crosses the -180/180 
   * boundary after it's converted, then we need to split it up to 
   * 2 polygons before we return the multipolygon. 
   * 
   * @param[in] poly360  (geos::geom::MultiPolygon)   polys split by 360 boundary
   * 
   * @return geos::geom::MultiPolygon  Returns a single polygon (Note: Longitude
   *                               coordinates will be less than 0.
   */

  geos::geom::MultiPolygon *PolygonTools::To180 (geos::geom::MultiPolygon *poly360) {

    geos::geom::CoordinateSequence *convPts;
    std::vector<geos::geom::Geometry *> polys;
    bool greater180 = false;
    bool less180 = false;
    //---------------------------------------------------------------------
    //We need to know if the poly360 came in as 1 or 2 polygons.
    //If it came in as 2 polys, then we need to union it before we return
    //because it was a polygon that crossed the 0/360 boundary.
    //---------------------------------------------------------------------
    int initialNumPolys = poly360->getNumGeometries();

    //------------------------------------------------------------------------
    // If single poly, and points in the single polygon are less than
    // 180 AND greater than 180, then we know we need to split this polygon up.
    //------------------------------------------------------------------------
    if (initialNumPolys == 1) {
      convPts = poly360->getGeometryN(0)->getCoordinates();

      //-----------------------------------------------------------------
      //Handling the case when a polygon will cross the -180/180 boundary
      //-----------------------------------------------------------------
      for (unsigned int i = 0; i < convPts->getSize(); i++) {
        if (convPts->getAt(i).x > 180) greater180 = true;
        if (convPts->getAt(i).x < 180) less180 = true;
      }

      //-----------------------------------------------------------------------
      // IF we do have a poly that crosses the -180/180 bndry...
      // THEN go thru the convPts and if any points are greater than 180, then
      // we'll put the > 180 in one poly and the < 180 in another poly and 
      // convert both.
      //----------------------------------------------------------------------
      if (greater180 && less180) {
        geos::geom::CoordinateSequence *pts1 = new geos::geom::DefaultCoordinateSequence ();
        geos::geom::CoordinateSequence *pts2 = new geos::geom::DefaultCoordinateSequence ();
        std::vector<geos::geom::Geometry *> case4polys;
        for (unsigned int i = 0; i < convPts->getSize(); i++) {
          if (convPts->getAt(i).x <= 180) {
            pts1->add (geos::geom::Coordinate(convPts->getAt(i).x,convPts->getAt(i).y));
          }
          if (convPts->getAt(i).x >= 180) {
            pts2->add (geos::geom::Coordinate(convPts->getAt(i).x,convPts->getAt(i).y));
          }
        }
       
        pts1->add (geos::geom::Coordinate(pts1->getX(0),pts1->getY(0)));
        pts2->add (geos::geom::Coordinate(pts2->getX(0),pts2->getY(0)));

        case4polys.push_back (Isis::globalFactory.createPolygon
                              (Isis::globalFactory.createLinearRing(pts1),NULL));
        case4polys.push_back (Isis::globalFactory.createPolygon
                              (Isis::globalFactory.createLinearRing(pts2),NULL));
        poly360 = Isis::globalFactory.createMultiPolygon (case4polys);
      }
    }

    //--------------------------------------------------------
    // If we still only have one single poly, simply convert 
    // coordinates to 180 system
    //--------------------------------------------------------
    if (poly360->getNumGeometries() == 1) {
      convPts = poly360->getGeometryN(0)->getCoordinates();
    }
    else {
      //  Find the poly that needs to be converted to 180 coordinates
      if ((poly360->getGeometryN(0)->getCoordinate()->x - 
           poly360->getGeometryN(1)->getCoordinate()->x) > 0) {
        polys.push_back(poly360->getGeometryN(1)->clone());
        convPts = poly360->getGeometryN(0)->getCoordinates();
      }
      else {
        polys.push_back(poly360->getGeometryN(0)->clone());
        convPts = poly360->getGeometryN(1)->getCoordinates();
        for (unsigned int i=0; i<convPts->getSize(); i++) {
        }
      }
    }

    //Convert poly to greater than 180
    geos::geom::CoordinateSequence *newPts = new geos::geom::CoordinateArraySequence();
    for (unsigned int i=0; i<convPts->getSize(); i++) {
      if(convPts->getAt(i).x > 180){
        newPts->add (geos::geom::Coordinate(convPts->getAt(i).x - 360.,convPts->getAt(i).y));
      } else {
        newPts->add (geos::geom::Coordinate(convPts->getAt(i).x,convPts->getAt(i).y));
      }
    }
   
    polys.push_back (Isis::globalFactory.createPolygon
                    (Isis::globalFactory.createLinearRing(newPts),NULL));

    //--------------------------------------------------------------
    //If the poly360 was initially 2 polygons, then we know that we
    //need to union them now.
    //--------------------------------------------------------------
    if(initialNumPolys > 1) {
      geos::geom::GeometryCollection *polyCollection = 
                Isis::globalFactory.createGeometryCollection(polys);
      geos::geom::Geometry *unionPoly = polyCollection->buffer(0);
      return (geos::geom::MultiPolygon *) unionPoly;
    } else {
      return Isis::globalFactory.createMultiPolygon (polys);
    }
  }


  /** 
   * Calculates the thickness of a polygon using:
   *      thickness = area / max(Xextent,Yextent)**2
   * The provided polygon SHOULD be an XY polygon, not a lat/lon polygon, but this 
   * is not enforced.
   * 
   * @param mpolygon The XY polygon to calculate the thickness of
   * 
   * @return double The thikness of the provided polygon
   */
  double PolygonTools::Thickness(const geos::geom::MultiPolygon *mpolygon ) {
    const geos::geom::Envelope *envelope = mpolygon->getEnvelopeInternal();

    double x = fabs( envelope->getMaxX() - envelope->getMinX() );
    double y = fabs( envelope->getMaxY() - envelope->getMinY() );
    double extent = max( x, y );

    return mpolygon->getArea() / (extent*extent);
  }


  /**
   * This method attempts to convert the geom to a MultiPolygon and then despike 
   * it. 
   *  
   * This method does not take ownership of the argument geom. The ownership 
   * of the return value is given to the caller. 
   *  
   * @param geom 
   * 
   * @return geos::geom::MultiPolygon* 
   */
  geos::geom::MultiPolygon* PolygonTools::Despike (const geos::geom::Geometry *geom) {
    geos::geom::MultiPolygon *spiked = MakeMultiPolygon(geom);
    geos::geom::MultiPolygon *despiked = Despike(spiked);

    delete spiked;
    spiked = NULL;

    return despiked;
  }

  /**
   * Create a new multipolygon without the spikes associated with
   * some versions of the geos package. 
   *  
   * This method does not take ownership of the argument multiPoly. The ownership 
   * of the return value is given to the caller. 
   *
   * @param multiPoly The original geos::geom::MultiPolygon to be despiked. 
   *  
   */
  geos::geom::MultiPolygon* PolygonTools::Despike (const geos::geom::MultiPolygon *multiPoly) {
    // Despike each polygon in the multipolygon
    vector<geos::geom::Geometry*> *newPolys = new vector<geos::geom::Geometry*>;
    for (unsigned int g=0; g<multiPoly->getNumGeometries(); ++g) {
      geos::geom::Polygon *poly = (geos::geom::Polygon*)(multiPoly->getGeometryN(g));

      // Despike each hole inside this polygon
      vector<geos::geom::Geometry *> *holes = new vector<geos::geom::Geometry *>;
      for (unsigned int h=0; h<poly->getNumInteriorRing(); ++h) {
        const geos::geom::LineString *ls = poly->getInteriorRingN(h);
        geos::geom::LinearRing *lr;

        // If the hole is not valid fix it
        // If the hole is NOT valid despike it
        lr = Despike(ls);

        if(!lr->isValid()) {
          geos::geom::LinearRing *fixed = FixGeometry(lr);
          delete lr;
          lr = fixed;
        }

        // Save this hole if it is not too small
        if (!lr->isEmpty()) {
          holes->push_back(lr);
        }
        else {
          delete lr;
        }
      } // End holes loop

      // Work on the main polygon
      const geos::geom::LineString* ls = poly->getExteriorRing();
      geos::geom::LinearRing *lr;

      lr = Despike(ls);

      try {
        if(!lr->isValid()) {
          geos::geom::LinearRing *fixed = FixGeometry(lr);
          delete lr;
          lr = fixed;
        }
      }
      catch(iException &e) {
        // Sometimes despike and fix fail, but the input is really valid. We can just go 
        // with the non-despiked polygon.
        if(ls->isValid() && ls->getGeometryTypeId() == geos::geom::GEOS_LINEARRING) {
          lr = (geos::geom::LinearRing *)ls->clone();
          e.Clear();
        }
        else {
          throw;
        }
      }

      // Create a new polygon with the holes and save it
      if (!lr->isEmpty()) {
        geos::geom::Polygon *tp = Isis::globalFactory.createPolygon(lr, holes);

        if (tp->isEmpty() || !tp->isValid()) {
          delete tp;
          newPolys->push_back (poly->clone());
        }
        else {
          newPolys->push_back (tp);
        }
      }
    } // End polygons loop

    // Create a new multipoly from the polygon(s)
    geos::geom::MultiPolygon *mp = Isis::globalFactory.createMultiPolygon(newPolys);

    if(!mp->isValid() || mp->isEmpty()) {
      delete mp;
      mp = NULL;
      iString msg = "Despike failed to correct the polygon";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // if multipoly area changes more than 25% we did something bad to the multipolygon
    if(fabs((mp->getArea() / multiPoly->getArea()) - 1.0) > 0.50) {
      iString msg = "Despike failed to correct the polygon " + mp->toString();
      delete mp;
      
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return mp;
  }


  /**
   * Create a new LinearRing from a LineString without the spikes associated
   * with some versions of the geos package. These "spikes" are created when 
   * intersections and differences are calculated. 
   *  
   * @param lineString The original geos::geom::lineString to be despiked. This 
   * can be the lineString associated with the outside of a polygon or one of its 
   * holes. 
   *  
   * @return A pointer to a LinearRing. If the despiking causes the number of 
   *         unique verticies to fall below 3, an empty LinearRing will be
   *         returned.
   */
  geos::geom::LinearRing* PolygonTools::Despike (const geos::geom::LineString *lineString) {
    geos::geom::CoordinateSequence *vertices = lineString->getCoordinates();

    // We need a full polygon to despike = 3 points (+1 for end==first) = at least 4 points
    if (vertices->getSize() < 4) {
      delete vertices;
      return Isis::globalFactory.createLinearRing (geos::geom::CoordinateArraySequence());
    }

    // delete one of the duplicate first/end coordinates,
    //   spikes can occur here and the duplicate points throws off the test
    vertices->deleteAt(vertices->getSize()-1);

    // Index will become negative if our first points are spiked, so we need
    //   to make it big enough to encapsulate the (unsigned int) from getSize()
    //   and allow negative numbers.
    for(long index = 0; index < (long)vertices->getSize(); index++) {
      // If we're under 3 vertices, we've despiked all the points out of the polygon :(
      if(vertices->getSize() < 3) {
        break;
      }

      // These are the array indices that we're going to test for a spike,
      //   the middle one is the one that's spiked if IsSpiked(...) is true.
      long testCoords[3] = {
        index-1,
        index,
        index+1
      };

      // Make sure the index is inside of our coordinates array (we'll have both too small/too big of an index) 
      for(int j = 0; j < 3; j++) {
        while(testCoords[j] < 0) {
          testCoords[j] += vertices->getSize();
        }

        while(testCoords[j] >= (long)vertices->getSize()) {
          testCoords[j] -= vertices->getSize();
        }
      }

      // Test the middle point for a spike
      if(IsSpiked(vertices->getAt(testCoords[0]), 
                  vertices->getAt(testCoords[1]), 
                  vertices->getAt(testCoords[2]))) {
        // It's spiked, delete it
        vertices->deleteAt(testCoords[1]);

        // Back up to the first test that is affected by this change
        index -= 2;
      }
    }

    if (vertices->getSize() < 3) {
      delete vertices;
      vertices = NULL;

      return Isis::globalFactory.createLinearRing (geos::geom::CoordinateArraySequence());
    }
    else {
      // Duplicate the first vertex as the last to close the polygon
      vertices->add(vertices->getAt(0));
      return Isis::globalFactory.createLinearRing(vertices);
    }
  }

  /**
   * Returns true if the middle point is spiked.
   * 
   * @param first 
   * @param middle 
   * @param last 
   * 
   * @return bool Returns true if middle point is spiked
   */
  bool PolygonTools::IsSpiked(geos::geom::Coordinate first, geos::geom::Coordinate middle, geos::geom::Coordinate last) {
    return TestSpiked(first, middle, last) || TestSpiked(last, middle, first);
  }

  /**
   * This method tests for spikes. The first/last coordinate matter. If the line 
   * between the first point and middle point are the base of a triangle, with the 
   * last point as the tip, then if the base of this triangle is substantially 
   * larger than the height we have a spike (fig A). If the middle point is near 
   * the edges of the triangle, however, this is valid data (fig B/C).
   *  
   *  (A)             (B)                 (C)
   *   1               1                   1-2
   *   |               |                     |
   *   |               |                     |
   * 3-|               |                     |
   *   |               |                     |
   *   |               |                     |
   *   2               2-3                   3
   *  
   *   *1 is start, 2 is middle, 3 is end
   *  
   *   Spikes are a problem because when we convert from lat/lons to meters the
   *   points get shifted relative to each other (lower points might be shifted
   *   more right then higher points, for example).
   * 
   * @param first 
   * @param middle 
   * @param last 
   * 
   * @return bool Returns true if middle point spiked given this first/last pt
   */
  bool PolygonTools::TestSpiked(geos::geom::Coordinate first, geos::geom::Coordinate middle, geos::geom::Coordinate last) {
    geos::geom::Point *firstPt = Isis::globalFactory.createPoint(first);
    geos::geom::Point *middlePt = Isis::globalFactory.createPoint(middle);
    geos::geom::Point *lastPt = Isis::globalFactory.createPoint(last);

    geos::geom::CoordinateSequence *coords = new geos::geom::CoordinateArraySequence();
    coords->add(first);
    coords->add(middle);
    geos::geom::LineString *line = Isis::globalFactory.createLineString(coords); // line takes ownership

    // The lower the tolerance, the less this algorithm removes and thus
    //   the better chance of success in findimageoverlaps. However, if you
    //   lower the tolerance then there is also a greater chance of programs
    //   such as autoseed failing. 1% is the current tolerance.
    double tolerance = line->getLength() / 100.0;

    bool spiked = true;

    double distanceLastMiddle = geos::operation::distance::DistanceOp::distance(lastPt, middlePt);
    double distanceLastLine = geos::operation::distance::DistanceOp::distance(lastPt, line);

    if(distanceLastMiddle == 0.0) return true; // get rid of same point

    // Checks the ratio of the distance between the last point and the line, and the last point and the middle point
    // if the ratio is very small then there is a spike
    if(distanceLastLine / distanceLastMiddle >= .05) {
      spiked = false;
    }

    // If the point is away from the line, keep it
    if (spiked && distanceLastLine > tolerance) {
      spiked = false;
    }

    if(!spiked) {
      geos::geom::CoordinateSequence *coords = new geos::geom::CoordinateArraySequence();
      coords->add(first);
      coords->add(middle);
      coords->add(last);
      coords->add(first);

      // shell takes ownership of coords
      geos::geom::LinearRing *shell = Isis::globalFactory.createLinearRing(coords);
      std::vector<geos::geom::Geometry*> *empty = new std::vector<geos::geom::Geometry*>;

      // poly takes ownership of shell and empty
      geos::geom::Polygon *poly = Isis::globalFactory.createPolygon(shell, empty);
     

      // if these 3 points define a straight line then the middle is worthless (defines nothing) or problematic
      if(poly->getArea() < 1.0e-10) {
        spiked = true;
      }

      delete poly;
    }
    

    delete firstPt;
    delete middlePt;
    delete lastPt;
    delete line;

    return spiked;
  }


  /**
   * This applies the geos Intersect operator. Due to "BinaryOp.h" having 
   * implementations in it, only one Isis object may perform these operations. If 
   * that file is included anywhere else in Isis, the library will not build! 
   * Please use this method to intersect two geometries. If the geometry is a 
   * linear ring or multi polygon, corrections may be applied if the geos 
   * intersection operator fails. 
   * 
   * @param geom1 First geometry to intersect
   * @param geom2 Second geometry to intersect
   * 
   * @return geos::geom::Geometry* geom1 intersected with geom2
   */
  geos::geom::Geometry *PolygonTools::Intersect(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) {
    try {
      return Operate(geom1, geom2, (unsigned int)geos::operation::overlay::OverlayOp::opINTERSECTION);
    }
    catch (geos::util::GEOSException *exc) {
      iString msg = "Intersect operation failed. The reason given was [" + iString(exc->what()) + "]";
      delete exc;
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    catch (iException &e) {
      iString msg = "Intersect operation failed";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    catch (...) {
      iString msg = "Intersect operation failed for an unknown reason";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  geos::geom::Geometry *PolygonTools::Operate(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2, unsigned int opcode) {

    geos::operation::overlay::OverlayOp::OpCode code = 
      (geos::operation::overlay::OverlayOp::OpCode)opcode;

    geos::geom::Geometry *result = NULL;
    bool failed = true;
    geos::geom::Geometry *geomFirst  = MakeMultiPolygon(geom1);
    geos::geom::Geometry *geomSecond = MakeMultiPolygon(geom2);

    geos::operation::overlay::snap::GeometrySnapper snap(*geomFirst);
    geos::geom::Geometry *geomSnapped = snap.snapTo(*geomSecond, 1.0e-10)->clone();
    if(!geomSnapped->isValid()) {
      delete geomSnapped;
    }
    else {
      delete geomFirst;
      geomFirst = geomSnapped;
    }

    unsigned int precision = 15;
    unsigned int minPrecision = 13;
    while(failed) {
      try {
        std::auto_ptr< geos::geom::Geometry > resultAuto = 
          BinaryOp(geomFirst, geomSecond, geos::operation::overlay::overlayOp(code));
        failed = false;
        result = resultAuto->clone();
      }
      catch(geos::util::GEOSException *exc) {
        // Just in case the clone failed....
        if(!failed || precision == minPrecision) throw;

        delete exc;
      }
      catch(...) {
        if(precision == minPrecision) {
          iString msg = "An unknown geos error occurred";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        if(!failed) {
          iString msg = "An unknown geos error occurred when attempting to clone a geometry";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }

      // try reducing the precision
      if(failed) {
        precision --;
        geos::geom::Geometry *tmp = ReducePrecision(geomFirst, precision);
        delete geomFirst;
        geomFirst = tmp;

        tmp = ReducePrecision(geomSecond, precision);
        delete geomSecond;
        geomSecond = tmp;
      }
    }

    delete geomFirst;
    geomFirst = NULL;

    delete geomSecond;
    geomSecond = NULL;

    if(result && !result->isValid()) {
      try {
        geos::geom::Geometry *newResult = FixGeometry(result);

        if(fabs(newResult->getArea() / result->getArea() - 1.0) > 0.50) {
          delete newResult;
          delete result;

          iString msg = "Operation [" + iString((int)opcode) + "] failed";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        delete result;
        result = newResult;
      }
      catch (iException &e) {
        iString msg = "Operation [" + iString((int)opcode) + "] failed";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }

    if(result == NULL) {
      iString msg = "Operation [" + iString((int)opcode) + " failed";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return result;
  }

  /**
   * This method attempts to seek out known problems with geometries and repair 
   * them. Currently the only known problem is when two points lie on top of each 
   * other, which is a result of various operations. Currently only LinearRings 
   * and MultiPolygons are supported. 
   * 
   * @param geom The geometry to be fixed
   * 
   * @return geos::geom::Geometry* A fixed geometry.
   */
  geos::geom::Geometry *PolygonTools::FixGeometry(const geos::geom::Geometry *geom) {
    if(geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON) {
      return FixGeometry( (geos::geom::MultiPolygon*)geom );
    }
    if(geom->getGeometryTypeId() == geos::geom::GEOS_LINEARRING) {
      return FixGeometry( (geos::geom::LinearRing*)geom );
    }
    if(geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON) {
      return FixGeometry( (geos::geom::Polygon*)geom );
    }
    if(geom->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION) {
      return FixGeometry( MakeMultiPolygon(geom) );
    }
    else {
      iString msg = "PolygonTools::FixGeometry does not support [" + GetGeometryName(geom) + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * This applies the LinearRing FixGeometry method to all of the interior and 
   * exterior rings in the multipolygon. See FixGeometry(LinearRing) for more 
   * explanation. 
   * 
   * @param poly An invalid multipolygon
   * 
   * @return geos::geom::MultiPolygon* A possibly valid multipolygon
   */
  geos::geom::MultiPolygon *PolygonTools::FixGeometry(const geos::geom::MultiPolygon *poly) {
    // Maybe two points are on top of each other
    vector<geos::geom::Geometry*> *newPolys = new vector<geos::geom::Geometry*>;

    // Convert each polygon in this multi-polygon
    for (unsigned int geomIndex = 0; geomIndex < poly->getNumGeometries(); geomIndex ++) {
      geos::geom::Polygon* fixedpoly = FixGeometry((geos::geom::Polygon*)(poly->getGeometryN(geomIndex)));
      if( fixedpoly->isValid() ) {
        newPolys->push_back( fixedpoly );
      }
      else {
        delete fixedpoly;
      }
      fixedpoly = NULL;
    }

    geos::geom::MultiPolygon *mp = Isis::globalFactory.createMultiPolygon(newPolys);
    return mp;
  }


  geos::geom::Polygon *PolygonTools::FixGeometry(const geos::geom::Polygon *poly) {

    // Convert each hole inside this polygon
    vector<geos::geom::Geometry *> *holes = new vector<geos::geom::Geometry *>;
    for (unsigned int holeIndex = 0; holeIndex < poly->getNumInteriorRing(); holeIndex ++) {
      const geos::geom::LineString *thisHole = poly->getInteriorRingN(holeIndex);

      // We hope they are all linear rings (closed/simple), but if not just leave it be
      if(thisHole->getGeometryTypeId() != geos::geom::GEOS_LINEARRING) {
        holes->push_back(thisHole->clone());
        continue;
      }

      try {
        geos::geom::LinearRing *newHole = FixGeometry((geos::geom::LinearRing *)thisHole);
        holes->push_back(newHole);
      }
      catch (iException &e) {
        iString msg = "Failed when attempting to fix interior ring of multipolygon";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    } // end num holes in polygon loop


    const geos::geom::LineString *exterior = poly->getExteriorRing();

    try {
      geos::geom::LinearRing *newExterior = NULL;

      if(exterior->getGeometryTypeId() == geos::geom::GEOS_LINEARRING) {
        newExterior = FixGeometry((geos::geom::LinearRing *)exterior);
      }
      else {
        iString msg = "Failed when attempting to fix exterior ring of polygon. The exterior ring is not simple and closed";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      return globalFactory.createPolygon(newExterior, holes);
    }
    catch (iException &e) {
      iString msg = "Failed when attempting to fix exterior ring of polygon";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * One problem we know of seems to happen when two points are right on top of 
   * each other. We're going to look for those points and remove them, then see if 
   * the linear ring is valid. Assumes input fails its isValid() test. 
   *  
   * Point end up on top of each other for two (2) known reasons. First, Despike 
   * removes a spike that simply went back and forth. Second, the geos Intersect 
   * operator can return invalid polygons because of this problem. Every geometry 
   * thus far is being broken down into LinearRings and this method cleans those 
   * up. 
   * 
   * @param ring An invalid linear ring
   * 
   * @return geos::geom::LinearRing* A possibly valid linear ring
   */
  geos::geom::LinearRing *PolygonTools::FixGeometry(const geos::geom::LinearRing *ring) {

    geos::geom::CoordinateSequence *coords = ring->getCoordinates();

    // Check this, just in case
    if(coords->getSize() < 4) {
      return globalFactory.createLinearRing(new geos::geom::DefaultCoordinateSequence ());
    }

    geos::geom::CoordinateSequence *newCoords = new geos::geom::DefaultCoordinateSequence ();
    const geos::geom::Coordinate *lastCoordinate = &coords->getAt(0);
    newCoords->add(*lastCoordinate);

    // Convert each coordinate in this hole
    for (unsigned int coordIndex = 1; coordIndex < coords->getSize() - 1; coordIndex ++) {
      const geos::geom::Coordinate *thisCoordinate = &coords->getAt(coordIndex);

      // we're going to compare the decimal place of the current point to the decimal place
      //   of the difference, if they are drastically different then geos might not be seeing them
      //   correctly.
      double difference[2] = {
        lastCoordinate->x - thisCoordinate->x,
        lastCoordinate->y - thisCoordinate->y,
      };

      // geos isnt differentiating between points this close
      double minDiff = fabs(DecimalPlace(thisCoordinate->x) - DecimalPlace(difference[0]));

      minDiff = min(minDiff, fabs(DecimalPlace(thisCoordinate->y) - DecimalPlace(difference[1])));

      // Cases where the difference in one direction is exactly zero, and the other direction is next to zero
      //  appear often enough (especially in despike).
      if(difference[0] == 0.0 && difference[1] != 0.0) {
        // subtracted the two points, got deltaX = 0.0, use the y difference decimal place
        minDiff = fabs(DecimalPlace(thisCoordinate->y) - DecimalPlace(difference[1]));
      }
      else if(difference[1] == 0.0 && difference[0] != 0.0) {
        // subtracted the two points, got deltaY = 0.0, use the x difference decimal place
        minDiff = fabs(DecimalPlace(thisCoordinate->x) - DecimalPlace(difference[0]));
      }
      else if(difference[0] == 0.0 && difference[1] == 0.0) {
        // subtracted the two points, got 0.0, so it's same point... make sure it gets ignored!
        minDiff = 1E99;
      }

      // geos has a hard time differentiating when points get too close...
      if(minDiff <= 15) {
        newCoords->add(*thisCoordinate);
        lastCoordinate = thisCoordinate;
      }
    } // end num coords in hole loop

    newCoords->add(geos::geom::Coordinate(newCoords->getAt(0).x, newCoords->getAt(0).y));
    geos::geom::LinearRing *newRing = NULL;

    // Now that we've weeded out any bad coordinates, let's rebuild the geometry
    try {
      if(newCoords->getSize() > 3) {
        newRing = globalFactory.createLinearRing(newCoords);
      }
      else {
        delete newCoords;
        newCoords = NULL;
      }
    }
    catch (geos::util::GEOSException *exc) {
      delete exc;
      exc = NULL;

      iString msg = "Error when attempting to fix linear ring";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(newRing && !newRing->isValid() && ring->isValid()) {
      iString msg = "Failed when attempting to fix linear ring";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    else if(!newRing || !newRing->isValid()) {
      if(newRing) {
        delete newRing;
      }

      newRing = (geos::geom::LinearRing*)ring->clone();
    }

    return newRing;
  }


  /**
   * This returns the location of the decimal place in the number. This method 
   * is used as a helper method for FixGeometry(...), to help figure out how close 
   * two numbers are in significant figures. 
   *  
   * 1.0 = decimal place 1, .1 = decimal place 0, 10.0 = decimal place 2
   * 
   * @param num The number to find the decimal place in
   * 
   * @return int The decimal place relative to the first significant digit
   */
  int PolygonTools::DecimalPlace(double num) {
    // 0.0 = decimal place 0
    if(num == 0.0) return 0;

    num = fabs(num);

    int decimalPlace = 1;
    while(num < 1.0) {
      num *= 10.0;
      decimalPlace --;
    }

    while(num > 10.0) {
      num /= 10.0;
      decimalPlace ++;
    }

    return decimalPlace;
  }

  /**
   * This method is used to subtract two polygons.
   * 
   * @param geom1 
   * @param geom2 
   * 
   * @return geos::geom::Geometry* 
   */
  geos::geom::Geometry *PolygonTools::Difference(const geos::geom::Geometry *geom1, const geos::geom::Geometry *geom2) {
    try {
      return Operate(geom1, geom2, (unsigned int)geos::operation::overlay::OverlayOp::opDIFFERENCE);
    }
    catch (geos::util::GEOSException *exc) {
      iString msg = "Difference operation failed. The reason given was [" + iString(exc->what()) + "]";
      delete exc;
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    catch (iException &e) {
      iString msg = "Difference operation failed";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    catch (...) {
      iString msg = "Difference operation failed for an unknown reason";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Make a geos::geom::MultiPolygon out of the components of the argument
   *
   * Create a new geos::geom::MultiPolygon out of the general geometry that is
   * passed in. This can be useful after an intersection or some other
   * operator on two MultiPolygons. The result of the operator is often a
   * collection of different geometries such as points, lines, polygons...
   * This member extracts all polygons and multipolygons into a new
   * multipolygon. The original geometry is deleted. The resulting multipolygon is 
   * not necessarily valid. 
   * 
   * @param geom The geometry to be converted into a multipolygon
   */
  geos::geom::MultiPolygon* PolygonTools::MakeMultiPolygon (const geos::geom::Geometry *geom) {
    // The area of the geometry is too small, so just ignore it.
    if (geom->isEmpty()) {
      return Isis::globalFactory.createMultiPolygon();
    }

    else if (geom->getArea()-DBL_EPSILON <= DBL_EPSILON) {
      return Isis::globalFactory.createMultiPolygon();
    }

    else if (geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON) {
      return (geos::geom::MultiPolygon *)geom->clone();
    }

    else if (geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON) {
      vector<geos::geom::Geometry*> *polys = new vector<geos::geom::Geometry*>;
      polys->push_back(geom->clone());
      geos::geom::MultiPolygon *mp = Isis::globalFactory.createMultiPolygon(polys);
      return mp;
     }

    else if (geom->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION) {
      vector<geos::geom::Geometry*> polys;
      geos::geom::GeometryCollection *gc = (geos::geom::GeometryCollection*)geom;
      for (unsigned int i=0; i < gc->getNumGeometries(); ++i) {
        if ((gc->getGeometryN(i)->getGeometryTypeId() == geos::geom::GEOS_POLYGON) &&
            (gc->getGeometryN(i)->getArea()-DBL_EPSILON > DBL_EPSILON)) {
          polys.push_back(gc->getGeometryN(i)->clone());
        }
      }

      geos::geom::MultiPolygon *mp = Isis::globalFactory.createMultiPolygon (polys);
      if (mp->getArea()-DBL_EPSILON <= DBL_EPSILON) {
        delete mp;
        mp = Isis::globalFactory.createMultiPolygon();
      }

      return mp;
    }
    // All other geometry types are invalid so ignore them
    else {
      return Isis::globalFactory.createMultiPolygon();
    }
  }


  /**
   * This method reduces the precision of the geometry to precision significant 
   * figures. 
   * 
   * @param geom The geometry to reduce precision on 
   * @param precision The precision to reduce to
   * 
   * @return geos::geom::Geometry* The lower precision geometry
   */
  geos::geom::Geometry *PolygonTools::ReducePrecision(const geos::geom::Geometry *geom, unsigned int precision) {
    if(geom->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON) {
      return ReducePrecision( (geos::geom::MultiPolygon*)geom, precision );
    }
    if(geom->getGeometryTypeId() == geos::geom::GEOS_LINEARRING) {
      return ReducePrecision( (geos::geom::LinearRing*)geom, precision );
    }
    if(geom->getGeometryTypeId() == geos::geom::GEOS_POLYGON) {
      return ReducePrecision( (geos::geom::Polygon*)geom, precision );
    }
    if(geom->getGeometryTypeId() == geos::geom::GEOS_GEOMETRYCOLLECTION) {
      return ReducePrecision( MakeMultiPolygon(geom), precision );
    }
    else {
      iString msg = "PolygonTools::ReducePrecision does not support [" + GetGeometryName(geom) + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This method reduces the precision of the MultiPolygon to precision 
   * significant figures. 
   * 
   * @param poly The MultiPolygon to reduce precision on 
   * @param precision The precision to reduce to
   * 
   * @return geos::geom::MultiPolygon* The lower precision MultiPolygon
   */
  geos::geom::MultiPolygon *PolygonTools::ReducePrecision(const geos::geom::MultiPolygon *poly, unsigned int precision) {
    // Maybe two points are on top of each other
    vector<geos::geom::Geometry*> *newPolys = new vector<geos::geom::Geometry*>;

    // Convert each polygon in this multi-polygon
    for (unsigned int geomIndex = 0; geomIndex < poly->getNumGeometries(); geomIndex ++) {
      geos::geom::Geometry* lowerPrecision = ReducePrecision((geos::geom::Polygon*)(poly->getGeometryN(geomIndex)), precision);

      if(!lowerPrecision->isEmpty()) {
        newPolys->push_back(lowerPrecision);
      }
      else {
        delete lowerPrecision;
      }
    }

    geos::geom::MultiPolygon *mp = Isis::globalFactory.createMultiPolygon(newPolys);
    return mp;
  }


  /**
   * This method reduces the precision of the Polygon to precision significant 
   * figures. 
   * 
   * @param poly The polygon to reduce precision on 
   * @param precision The precision to reduce to
   * 
   * @return geos::geom::Polygon* The lower precision polygon
   */
  geos::geom::Polygon *PolygonTools::ReducePrecision(const geos::geom::Polygon *poly, unsigned int precision) {
    // Convert each hole inside this polygon
    vector<geos::geom::Geometry *> *holes = new vector<geos::geom::Geometry *>;
    for (unsigned int holeIndex = 0; holeIndex < poly->getNumInteriorRing(); holeIndex ++) {
      const geos::geom::LineString *thisHole = poly->getInteriorRingN(holeIndex);

      // We hope they are all linear rings (closed/simple), but if not just leave it be
      if(thisHole->getGeometryTypeId() != geos::geom::GEOS_LINEARRING) {
        holes->push_back(thisHole->clone());
        continue;
      }

      try {
        geos::geom::LinearRing *newHole = ReducePrecision((geos::geom::LinearRing *)thisHole, precision);

        if(!newHole->isEmpty()) {
          holes->push_back(newHole);
        }
        else {
          delete newHole;
        }
        
      }
      catch (iException &e) {
        iString msg = "Failed when attempting to fix interior ring of multipolygon";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    } // end num holes in polygon loop


    const geos::geom::LineString *exterior = poly->getExteriorRing();

    try {
      geos::geom::LinearRing *newExterior = NULL;

      if(exterior->getGeometryTypeId() == geos::geom::GEOS_LINEARRING) {
        newExterior = ReducePrecision((geos::geom::LinearRing *)exterior, precision);
      }
      else {
        iString msg = "Failed when attempting to fix exterior ring of polygon. The exterior ring is not simple and closed";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      return globalFactory.createPolygon(newExterior, holes);
    }
    catch (iException &e) {
      iString msg = "Failed when attempting to fix exterior ring of polygon";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This method reduces the precision of the LinearRing to precision significant 
   * figures. 
   * 
   * @param ring The linear ring to reduce precision on 
   * @param precision The precision to reduce to
   * 
   * @return geos::geom::LinearRing* The lower precision linear ring
   */
  geos::geom::LinearRing *PolygonTools::ReducePrecision(const geos::geom::LinearRing *ring, unsigned int precision) {
    geos::geom::CoordinateSequence *coords = ring->getCoordinates();

    // Check this, just in case
    if(coords->getSize() <= 0) return (geos::geom::LinearRing *)ring->clone();

    geos::geom::CoordinateSequence *newCoords = new geos::geom::DefaultCoordinateSequence ();
    geos::geom::Coordinate *coord = ReducePrecision(&coords->getAt(0), precision);
    newCoords->add(*coord);
    delete coord;
    coord = NULL;

    // Convert each coordinate in this ring
    for (unsigned int coordIndex = 1; coordIndex < coords->getSize() - 1; coordIndex ++) {
      const geos::geom::Coordinate *thisCoordinate = &coords->getAt(coordIndex);
      coord = ReducePrecision(thisCoordinate, precision);
      newCoords->add(*coord);
      delete coord;
      coord = NULL;
    }

    newCoords->add(geos::geom::Coordinate(newCoords->getAt(0).x, newCoords->getAt(0).y));
    geos::geom::LinearRing *newRing = NULL;

    // Now that we've weeded out any bad coordinates, let's rebuild the geometry
    try {
      newRing = globalFactory.createLinearRing(newCoords);
    }
    catch (geos::util::GEOSException *exc) {
      delete exc;
      exc = NULL;

      iString msg = "Error when attempting to reduce precision of linear ring";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // try to despike
    try {
      geos::geom::LinearRing *tmp = Despike(newRing);
      delete newRing;
      newRing = tmp;
    }
    catch (iException &e) {
      e.Clear();
    }

    if(!newRing->isValid()) {
      iString msg = "Failed when attempting to reduce precision of linear ring";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return newRing;
  }


  /**
   * This method reduces the precision of the coordinate to precision significant 
   * figures. 
   * 
   * @param coord The coordinate to reduce precision on 
   * @param precision The precision to reduce to
   * 
   * @return geos::geom::Coordinate* The lower precision coordinate
   */
  geos::geom::Coordinate *PolygonTools::ReducePrecision(const geos::geom::Coordinate *coord, unsigned int precision) {
    return new geos::geom::Coordinate(
      ReducePrecision(coord->x, precision), 
      ReducePrecision(coord->y, precision), 
      ReducePrecision(coord->z, precision)
      );
  }


  /**
   * This method will reduce the decimal precision of the inputted num to 
   * precision decimal places. 
   * 
   * @param num The original number
   * @param precision The new precision
   * 
   * @return double The reduced precision number
   */
  double PolygonTools::ReducePrecision(double num, unsigned int precision) {
    int decimalPlace = DecimalPlace(num);
    double factor = pow(10.0, (int)decimalPlace);

    // reduced num is in the form 0.nnnnnnnnnn...
    double reducedNum = num / factor;

    double cutoff = pow(10.0, (int)precision);
    double round_offset = (num < 0)? -0.5 : 0.5;

    // cast off the digits past the precision's place
    reducedNum = ((long long)(reducedNum * cutoff + round_offset)) / cutoff;

    return reducedNum*factor;
  }


  /**
   * This method returns the name of the type of geometry passed in. This is 
   * useful for error reporting (i.e. Geometry Type [...] not supported).
   * 
   * @param geom The geometry to test which type it really is
   * 
   * @return std::string 
   */
  std::string PolygonTools::GetGeometryName(const geos::geom::Geometry *geom) {
    switch(geom->getGeometryTypeId()) {
      case geos::geom::GEOS_POINT:
        return "Point";
      case geos::geom::GEOS_LINESTRING:
        return "Line String";
      case geos::geom::GEOS_LINEARRING:
        return "Linear Ring";
      case geos::geom::GEOS_POLYGON:
        return "Polygon";
      case geos::geom::GEOS_MULTIPOINT:
        return "Multi Point";
      case geos::geom::GEOS_MULTILINESTRING:
        return "Multi Line String";
      case geos::geom::GEOS_MULTIPOLYGON:
        return "Multi Polygon";
      case geos::geom::GEOS_GEOMETRYCOLLECTION:
        return "Geometry Collection";
      default:
        return "UNKNOWN";
    }
  }


  bool PolygonTools::Equal( const geos::geom::MultiPolygon * poly1, const geos::geom::MultiPolygon * poly2 ) {

    vector<const geos::geom::Polygon*> polys1;
    vector<const geos::geom::Polygon*> polys2;
    
    if (poly1->getNumGeometries() != poly2->getNumGeometries())  return false;

    // Convert each polygon in this multi-polygon
    for (unsigned int geomIndex = 0; geomIndex < poly1->getNumGeometries(); geomIndex ++) {
      polys1.push_back( (geos::geom::Polygon*)poly1->getGeometryN(geomIndex) );
      polys2.push_back( (geos::geom::Polygon*)poly2->getGeometryN(geomIndex) );
    }

    for (int p1 = polys1.size()-1; (p1 >= 0) && polys1.size(); p1 --) {
      for (int p2 = polys2.size()-1; (p2 >= 0) && polys2.size(); p2 --) {
        if ( Equal(polys1[p1],polys2[p2]) ) {
          // Delete polys1[p1] by replacing it with the last Polygon in polys1
          polys1[p1] = polys1[polys1.size()-1];
          polys1.resize( polys1.size()-1 );
          // Delete polys2[p2] by replacing it with the last Polygon in polys2
          polys2[p2] = polys2[polys2.size()-1];
          polys2.resize( polys2.size()-1 );
        }
      }
    }
    
    return (polys1.size() == 0) && (polys2.size() == 0);
  }


  bool PolygonTools::Equal( const geos::geom::Polygon * poly1, const geos::geom::Polygon * poly2 ) {
    vector<const geos::geom::LineString *> holes1;
    vector<const geos::geom::LineString *> holes2;

    if (poly1->getNumInteriorRing() != poly2->getNumInteriorRing())  return false;

    if ( !Equal(poly1->getExteriorRing(),poly2->getExteriorRing()) )  return false;

    // Convert each hole inside this polygon
    for (unsigned int holeIndex = 0; holeIndex < poly1->getNumInteriorRing(); holeIndex ++) {

      // We hope they are all linear rings (closed/simple), but if not just leave it be
      if(poly1->getInteriorRingN(holeIndex)->getGeometryTypeId() == geos::geom::GEOS_LINESTRING) {
        holes1.push_back( poly1->getInteriorRingN(holeIndex) );
      }

      if(poly2->getInteriorRingN(holeIndex)->getGeometryTypeId() == geos::geom::GEOS_LINESTRING) {
        holes2.push_back( poly2->getInteriorRingN(holeIndex) );
      }

    }

    if (holes1.size() != holes2.size())  return false;

    for (int h1 = holes1.size()-1; (h1 >= 0) && holes1.size(); h1 --) {
      for (int h2 = holes2.size()-1; (h2 >= 0) && holes2.size(); h2 --) {
        if ( Equal(holes1[h1],holes2[h2]) ) {
          // Delete holes1[h1] by replacing it with the last Polygon in holes1
          holes1[h1] = holes1[holes1.size()-1];
          holes1.resize( holes1.size()-1 );
          // Delete holes2[h2] by replacing it with the last Polygon in holes2
          holes2[h2] = holes2[holes2.size()-1];
          holes2.resize( holes2.size()-1 );
        }
      }
    }
      
    return (holes1.size() == 0) && (holes2.size() == 0);
  }


  bool PolygonTools::Equal( const geos::geom::LineString * lineString1, const geos::geom::LineString * lineString2 ) {

    geos::geom::CoordinateSequence *coords1 = lineString1->getCoordinates();
    geos::geom::CoordinateSequence *coords2 = lineString2->getCoordinates();
    bool isEqual = true;

    if (coords1->getSize() != coords2->getSize() ) isEqual = false;

    unsigned int index1 = 0;
    unsigned int index2 = 0;

    // -1 extra for dupicate start/end coordinates
    for( ; index2 < coords2->getSize()-1 && isEqual; index2 ++) {
      if (Equal(coords1->getAt(index1),coords2->getAt(index2)))  break;
    }

    if (index2 == coords2->getSize()-1) isEqual = false;

    for ( ; index1 < coords1->getSize()-1 && isEqual; index1 ++, index2 ++) {
      if (!Equal(coords1->getAt(index1),coords2->getAt(index2 % (coords2->getSize()-1)))) {
        isEqual = false;
      }
    }

    delete coords1;
    delete coords2;
    return isEqual;
  }


  bool PolygonTools::Equal( const geos::geom::Coordinate & coord1, const geos::geom::Coordinate & coord2 ) {

    if (!Equal(coord1.x,coord2.x))  return false;
    if (!Equal(coord1.y,coord2.y))  return false;
    if (!Equal(coord1.y,coord2.y))  return false;

    return true;
  }


  bool PolygonTools::Equal( const double d1, const double d2 ) {
    const double cutoff = 1e15;

    if(DecimalPlace(d1) != DecimalPlace(d2)) return false;

    int decimalPlace = DecimalPlace(d1);
    double factor = pow(10.0, (int)decimalPlace);

    // reduced num is in the form 0.nnnnnnnnnn...
    double reducedNum = d1 / factor;

    double round_offset = (d1 < 0)? -0.5 : 0.5;

    // cast off the digits past the precision's place
    long long num1 = ((long long)(reducedNum * cutoff + round_offset));

    factor = pow(10.0, (int)decimalPlace);

    // reduced num is in the form 0.nnnnnnnnnn...
    reducedNum = d2 / factor;

    round_offset = (d2 < 0)? -0.5 : 0.5;

    // cast off the digits past the precision's place
    long long num2 = ((long long)(reducedNum * cutoff + round_offset));


    return (num1 == num2);
  }


} // end namespace isis

