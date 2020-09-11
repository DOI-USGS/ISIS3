#include <iostream>
#include <QTemporaryFile>

#include "findimageoverlaps.h"
#include "Fixtures.h"
#include "TestUtilities.h"
#include "IException.h"
#include "FileList.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "ImageOverlap.h"
#include "ImageOverlapSet.h"
#include "Blob.h"
#include "Pvl.h"

#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestFindImageOverlapsNoOverlap) {
  ImagePolygon fp1;
  fp1.Create(*cube1);
  cube1->write(fp1);

  Cube newCube2;
  json newIsd2;
  std::ifstream i(isdPath2->expanded().toStdString());
  i >> newIsd2;

  newIsd2["instrument_position"]["positions"] = {{1,1,1}, {2,2,2}, {3,3,3}};
  newCube2.fromIsd(tempDir.path()+"/new2.cub", *cube2->label(), newIsd2, "rw");

  ImagePolygon fp2;
  fp2.Create(newCube2);
  newCube2.write(fp2);

  FileList cubes;
  cubes.append(cube1->fileName());
  cubes.append(newCube2.fileName());
  cube1->close();
  cube2->close();
  newCube2.close();

  QString cubeListPath = tempDir.path() + "/cubes.lis";
  cubes.write(cubeListPath);
  QVector<QString> args = {"from="+cubeListPath, "overlapList="+tempDir.path()+"/overlaps.txt"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  try {
    findimageoverlaps(options, &appLog);
    FAIL() << "Expected an IException with message: \"No overlaps were found\".";
  }
  catch(IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("No overlaps were found"))
      << e.toString().toStdString();
  }

}

TEST_F(ThreeImageNetwork, FunctionalTestFindImageOverlapTwoImageOverlap) {

  QVector<QString> args = {"OVERLAPLIST=" + tempDir.path() + "/overlaps.txt", "detailed=true", "errors=true"};
  UserInterface ui(APP_XML, args);
  FileList images;
  images.append(FileName(cube1->fileName()));
  images.append(FileName(cube2->fileName()));
  findimageoverlaps(images, ui, nullptr);

  // Find all the overlaps between the images in the FROMLIST
  // The overlap polygon coordinates are in Lon/Lat order
  ImageOverlapSet overlaps;
  overlaps.ReadImageOverlaps(ui.GetFileName("OVERLAPLIST"));
  ASSERT_EQ(overlaps.Size(), 3);
  const ImageOverlap *poi;
  const geos::geom::MultiPolygon *mp;

  poi = overlaps[0];
  mp = poi->Polygon();
  ASSERT_EQ(mp->getArea(), 14);
  ASSERT_EQ(poi->Size(), 1);
  ASSERT_EQ((*poi)[0], "MGS/688540926:0/MOC-WA/RED");

  poi = overlaps[1];
  mp = poi->Polygon();
  ASSERT_EQ(mp->getArea(), 14);
  ASSERT_EQ(poi->Size(), 1);
  ASSERT_EQ((*poi)[0], "MGS/691204200:96/MOC-WA/RED");

  poi = overlaps[2];
  mp = poi->Polygon();
  ASSERT_EQ(mp->getArea(), 36);
  ASSERT_EQ(poi->Size(), 2);
  ASSERT_EQ((*poi)[0], "MGS/688540926:0/MOC-WA/RED");
  ASSERT_EQ((*poi)[1], "MGS/691204200:96/MOC-WA/RED");
}

TEST_F(ThreeImageNetwork, FunctionalTestFindImageOverlapFullOverlap) {

  lonLatPts = new geos::geom::CoordinateArraySequence();
  lonLatPts->add(geos::geom::Coordinate(31, 1));
  lonLatPts->add(geos::geom::Coordinate(31, 9));
  lonLatPts->add(geos::geom::Coordinate(34, 9));
  lonLatPts->add(geos::geom::Coordinate(34, 1));
  lonLatPts->add(geos::geom::Coordinate(31, 1));

  polys = new std::vector<geos::geom::Geometry *>;
  poly = globalFactory->createPolygon(globalFactory->createLinearRing(lonLatPts), nullptr);
  polys->push_back(poly->clone());
  multiPoly = globalFactory->createMultiPolygon(polys);

  geos::io::WKTWriter *wkt = new geos::io::WKTWriter();

  std::string polyStr = wkt->write(multiPoly);
  int polyStrSize = polyStr.size();
  std::istringstream polyStream(polyStr);

  Blob pvlBlob("Footprint", "Polygon");
  Pvl pvl;
  PvlObject polyObject = PvlObject("Polygon");
  polyObject.addKeyword(PvlKeyword("Name", "Footprint"));
  polyObject.addKeyword(PvlKeyword("StartByte", "1"));
  polyObject.addKeyword(PvlKeyword("Bytes", toString(polyStrSize)));
  pvl.addObject(polyObject);

  pvlBlob.Read(pvl, polyStream);
  cube2->write(pvlBlob);
  cube2->reopen("rw");

  delete wkt;

  QVector<QString> args = {"OVERLAPLIST=" + tempDir.path() + "/overlaps.txt", "detailed=true", "errors=true"};
  UserInterface ui(APP_XML, args);
  FileList images;
  images.append(FileName(cube1->fileName()));
  images.append(FileName(cube2->fileName()));
  findimageoverlaps(images, ui, nullptr);

  // Find all the overlaps between the images in the FROMLIST
  // The overlap polygon coordinates are in Lon/Lat order
  ImageOverlapSet overlaps;
  overlaps.ReadImageOverlaps(ui.GetFileName("OVERLAPLIST"));
  ASSERT_EQ(overlaps.Size(), 2);
  const ImageOverlap *poi;
  const geos::geom::MultiPolygon *mp;

  poi = overlaps[0];
  mp = poi->Polygon();
  ASSERT_EQ(mp->getArea(), 26);
  ASSERT_EQ(poi->Size(), 1);
  ASSERT_EQ((*poi)[0], "MGS/688540926:0/MOC-WA/RED");

  poi = overlaps[1];
  mp = poi->Polygon();
  ASSERT_EQ(mp->getArea(), 24);
  ASSERT_EQ(poi->Size(), 2);
  ASSERT_EQ((*poi)[0], "MGS/691204200:96/MOC-WA/RED");
  ASSERT_EQ((*poi)[1], "MGS/688540926:0/MOC-WA/RED");
}
