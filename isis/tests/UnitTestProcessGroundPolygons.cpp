#include "ImagePolygon.h"
#include "LineManager.h"
#include "PolygonTools.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "geos/geom/Point.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateSequence.h"

#include <gtest/gtest.h>

#include "CameraFixtures.h"
#include "TempFixtures.h"

#include "ProcessGroundPolygons.h"

using namespace Isis;

// Default test to check integrity of the geometry
TEST( UnitTestVectorize, Default ) {
	
  /*	
  ImagePolygon poly;
  try {
    poly.Create(*testCube);
  }
  catch(IException &e) {
    QString msg = "Cannot create polygon for [" + testCube->fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
  ASSERT_EQ(4517, poly.numVertices());
  	

  geos::geom::Geometry* boundary = poly.Polys()->getEnvelope().release();
  geos::geom::Point* centroid = poly.Polys()->getCentroid().release();
  */
	
  Isis::ProcessGroundPolygons g_processGroundPolygons;
  geos::geom::Geometry* groundpixel;
  //geos::io::WKTWriter *wkt = new geos::io::WKTWriter();
 
  std::vector<double> lons = {255.645377, 256.146301, 256.146301, 255.645377};
  std::vector<double> lats = {9.928429, 9.928429, 10.434929, 10.434929};
  //std::vector<double> dns =  {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10};

  groundpixel = g_processGroundPolygons.Vectorize(lats, lons);

  // Check that the geometry is valid
  EXPECT_EQ( groundpixel->isValid(), 1 );
  EXPECT_EQ( groundpixel->isPolygonal(), 1 );
  EXPECT_EQ( groundpixel->getGeometryType(), "Polygon");
  EXPECT_EQ( groundpixel->getNumGeometries(), 1); 
  EXPECT_EQ( groundpixel->getNumPoints(), lons.size() + 1 );
  

  //geos::geom::CoordinateSequence coordArray = *(boundary->getCoordinates().release());
  
  //for (size_t i = 0; i < coordArray.getSize(); i++) {
  //  EXPECT_NEAR(lons[i], coordArray.getAt(i).x, 1e-6);
  //  EXPECT_NEAR(lats[i], coordArray.getAt(i).y, 1e-6);
  //}

  //EXPECT_NEAR(255.895201, centroid->getX(), 1e-6);
  //EXPECT_NEAR(10.182391, centroid->getY(), 1e-6);
  
  
};

TEST( UnitTestVectorize, Crosses360 ) {
	
    Isis::ProcessGroundPolygons g_processGroundPolygons;
    geos::geom::Geometry* groundpixel;
    //geos::io::WKTWriter *wkt = new geos::io::WKTWriter();
 
    
	
    std::vector<double> lons = {359.0,   1.0,   1.0, 359.0, 359.0};
    std::vector<double> lats = {  0.0,   0.0,   1.0,   1.0,   0.0};	
	
	
	/*
    std::vector<double> lons = {350, 351, 351, 350};
    std::vector<double> lats = {0  ,   0,   1,   1};
    */
	
	groundpixel = g_processGroundPolygons.Vectorize(lats, lons);
	
    // Check that the geometry is valid
    EXPECT_EQ( groundpixel->isValid(), 1 );
    EXPECT_EQ( groundpixel->isPolygonal(), 1 );
	EXPECT_EQ( groundpixel->getGeometryType(), "MultiPolygon");
    EXPECT_EQ( groundpixel->getNumGeometries(), 2); 

	
};