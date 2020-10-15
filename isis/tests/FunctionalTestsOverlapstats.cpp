#include <QTextStream>
#include <QStringList>
#include <QTemporaryFile>

#include "findimageoverlaps.h"
#include "overlapstats.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/overlapstats.xml").expanded();

// trigger "Unable to convert polygon from Lat/Lon to X/Y:" error
// test that the errors are written to the specified error file
TEST_F(ThreeImageNetwork, FunctionalTestOverlapstatsErrors) {
  // generate footprint with bowtie hole in it for cube3
  geos::geom::CoordinateSequence *ptsA, *ptsA_hole;
  geos::geom::Polygon *polyA;

  ptsA = new geos::geom::CoordinateArraySequence();
  ptsA->add(geos::geom::Coordinate(31, 1));
  ptsA->add(geos::geom::Coordinate(35, 1));
  ptsA->add(geos::geom::Coordinate(35, 10));
  ptsA->add(geos::geom::Coordinate(31, 10));
  ptsA->add(geos::geom::Coordinate(31, 1));

  ptsA_hole = new geos::geom::CoordinateArraySequence();
  ptsA_hole->add(geos::geom::Coordinate(32, 2));
  ptsA_hole->add(geos::geom::Coordinate(32, 9));
  ptsA_hole->add(geos::geom::Coordinate(34, 9));
  ptsA_hole->add(geos::geom::Coordinate(34, 2));
  ptsA_hole->add(geos::geom::Coordinate(32, 2));


  std::vector<geos::geom::Geometry *> *holes = new std::vector<geos::geom::Geometry *>;
  holes->push_back(globalFactory->createLinearRing(ptsA_hole));
  polys = new std::vector<geos::geom::Geometry *>;
  polyA = globalFactory->createPolygon(globalFactory->createLinearRing(ptsA), holes);
  polys->push_back(polyA->clone());
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
  cube3->write(pvlBlob);
  cube3->reopen("rw");

  QString cubeListPath = "/home/tgiroux/Desktop/ols/cubes.lis";
  QString overlapsPath = "/home/tgiroux/Desktop/ols/overlaps.lis";
  QString errorPath = "/home/tgiroux/Desktop/ols/errors.txt";


  // last overlap is a bowtie shape inside of other overlaps
  // fails in overlapstats.cpp: overlaps.ReadImageOverlaps(...)
  QString overlaps = R"(MGS/688540926:0/MOC-WA/RED
010600000001000000010300000001000000070000000000000000003E4000000000000000000000000000003E4000000000000024400000000000003F4000000000000024400000000000003F40000000000000F03F0000000000804140000000000000F03F000000000080414000000000000000000000000000003E400000000000000000
MGS/691204200:96/MOC-WA/RED
010600000001000000010300000001000000070000000000000000003F4000000000000024400000000000003F400000000000002640000000000000424000000000000026400000000000004240000000000000F03F0000000000804140000000000000F03F000000000080414000000000000024400000000000003F400000000000002440
MGS/688540926:0/MOC-WA/RED,MGS/691204200:96/MOC-WA/RED
010600000001000000010300000001000000050000000000000000003F40000000000000F03F0000000000003F400000000000002440000000000080414000000000000024400000000000804140000000000000F03F0000000000003F40000000000000F03F
MGS/688540926:0/MOC-WA/RED,MGS/691204200:96/MOC-WA/RED
010600000001000000010300000002000000050000000000000000003F40000000000000F03F0000000000003F400000000000002440000000000080414000000000000024400000000000804140000000000000F03F0000000000003F40000000000000F03F050000000000000000004040000000000000004000000000000041400000000000002240000000000000414000000000000000400000000000004040000000000000224000000000000040400000000000000040
)"; 
  // write the above to overlaps to use as input for overlapstats    
  QFile overlapsFile(overlapsPath); 
  if (overlapsFile.open(QIODevice::WriteOnly)) {
    QTextStream out(&overlapsFile); 
    out << overlaps;
    overlapsFile.close(); 
  }
  else { 
    FAIL() << "Failed to create overlaps file" << std::endl;
  }

  FileList cubes;
  cubes.append(FileName(cube1->fileName()));
  cubes.append(FileName(cube2->fileName()));
  // cubes.append(FileName(cube3->fileName())); // cube3 has a bowtie hole in it, this causes findimageoverlaps to fail
  cube1->close();
  cube2->close();
  cube3->close();
  cubes.write(cubeListPath);

/*
  QString FIO_APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
  QVector<QString> fio_args = { "OVERLAPLIST=" + overlapsPath };
  UserInterface fioOptions(FIO_APP_XML, fio_args);
  findimageoverlaps(cubes, fioOptions);
*/

  QVector<QString> args = { "FROMLIST=" + cubeListPath,
                            "OVERLAPLIST=" + overlapsPath,
                            "ERRORS=" + errorPath };
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QFile errorFile(errorPath);

  int sizeBefore = errorFile.size();
  overlapstats(options, &appLog);
  ASSERT_GT(errorFile.size(), sizeBefore);
}


TEST_F(ThreeImageNetwork, FunctionalTestOverlapstatsBadCubeList) {
  QString badCubeList = tempDir.path() + "/badcubes.lis";
  QString overlapPath = tempDir.path() + "/overlaps.lis";
  FileList cubes;

  // badCubeList only contains cube1, overlaps has cube1 and cube2
  cubes.append(FileName(cube1->fileName()));
  cubes.write(badCubeList);
  cubes.append(FileName(cube2->fileName()));
  cube1->close();
  cube2->close();

  // get overlaps from findimageoverlaps
  QString FIO_APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
  QVector<QString> fio_args = { "OVERLAPLIST=" + overlapPath };
  UserInterface fioOptions(FIO_APP_XML, fio_args);
  findimageoverlaps(cubes, fioOptions, false, nullptr);

  QVector<QString> args = { "FROMLIST=" + badCubeList,
                            "OVERLAPLIST=" + overlapPath };
  UserInterface options(APP_XML, args);
  Pvl appLog;

  try {
    overlapstats(options, &appLog);
    FAIL() << "Expected Exception for invalid overlap list";
  }
  catch(IException &e) {
    QString errorMsg = "in overlap list that was not in the provided cube list";
    EXPECT_TRUE(e.toString().toLatin1().contains(errorMsg.toLatin1()))
                                              << e.toString().toStdString();
  }
}


TEST_F(ThreeImageNetwork, FunctionalTestOverlapstatsDefault) {
  // fill points so cube3 contributes to overlaps
  lonLatPts = new geos::geom::CoordinateArraySequence();
  lonLatPts->add(geos::geom::Coordinate(30, 0));
  lonLatPts->add(geos::geom::Coordinate(30, 10));
  lonLatPts->add(geos::geom::Coordinate(35, 10));
  lonLatPts->add(geos::geom::Coordinate(35, 0));
  lonLatPts->add(geos::geom::Coordinate(30, 0));

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
  cube3->write(pvlBlob);
  cube3->reopen("rw");

  FileList cubes;
  cubes.append(FileName(cube1->fileName()));
  cubes.append(FileName(cube2->fileName()));
  cubes.append(FileName(cube3->fileName()));
  cube1->close();
  cube2->close();
  cube3->close();

  QString cubeListPath = tempDir.path() + "/cubes.lis";
  QString overlapPath = tempDir.path() + "/overlaps.lis";
  cubes.write(cubeListPath);

  // get overlaps from findimageoverlaps
  QString FIO_APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
  QVector<QString> fio_args = { "OVERLAPLIST=" + overlapPath };
  UserInterface fioOptions(FIO_APP_XML, fio_args);
  findimageoverlaps(cubes, fioOptions);

  QVector<QString> args = { "FROMLIST=" + cubeListPath,
                            "OVERLAPLIST=" + overlapPath };
  UserInterface options(APP_XML, args);
  Pvl appLog;
  overlapstats(options, &appLog);

  PvlGroup group = appLog.findGroup("Results");
  EXPECT_EQ( (double) group.findKeyword("ThicknessMinimum"), 0.14135606732925);
  EXPECT_EQ( (double) group.findKeyword("ThicknessMaximum"), 0.44103454403747);
  EXPECT_EQ( (double) group.findKeyword("ThicknessAverage"), 0.29119530568336);
  EXPECT_EQ( (double) group.findKeyword("ThicknessStandardDeviation"), 0.21190468305603);
  EXPECT_EQ( (double) group.findKeyword("ThicknessVariance"), 0.044903594701078);
  EXPECT_EQ( (double) group.findKeyword("AreaMinimum"), 49665309599.111);
  EXPECT_EQ( (double) group.findKeyword("AreaMaximum"), 125515198928.3);
  EXPECT_EQ( (double) group.findKeyword("AreaAverage"), 87590254263.708);
  EXPECT_EQ( (double) group.findKeyword("AreaStandardDeviation"), 53633971096.922);
  EXPECT_EQ( (double) group.findKeyword("AreaVariance"), 2.87660285562549e+21);
  EXPECT_EQ( (double) group.findKeyword("ImageStackMinimum"), 2.0);
  EXPECT_EQ( (double) group.findKeyword("ImageStackMaximum"), 3.0);
  EXPECT_EQ( (double) group.findKeyword("ImageStackAverage"), 2.5);
  EXPECT_EQ( (double) group.findKeyword("ImageStackStandardDeviation"), 0.70710678118655);
  EXPECT_EQ( (double) group.findKeyword("ImageStackVariance"), 0.5);
  EXPECT_EQ( (int) group.findKeyword("PolygonCount"), 3);
}


TEST_F(ThreeImageNetwork, FunctionalTestOverlapstatsFull) {
  // write points so that cube2 is a full overlap of cube1
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

  FileList cubes;
  cubes.append(FileName(cube1->fileName()));
  cubes.append(FileName(cube2->fileName()));
  cube1->close();
  cube2->close();

  QString cubeListPath = tempDir.path() + "/cubes.lis";
  QString overlapPath = tempDir.path() + "/overlaps.lis";
  cubes.write(cubeListPath);

  // get overlaps for overlapstats input
  QString FIO_APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
  QVector<QString> fio_args = { "OVERLAPLIST=" + overlapPath };
  UserInterface fioOptions(FIO_APP_XML, fio_args);
  findimageoverlaps(cubes, fioOptions);

  QVector<QString> args = { "FROMLIST=" + cubeListPath,
                            "OVERLAPLIST=" + overlapPath };
  UserInterface options(APP_XML, args);
  Pvl appLog;
  overlapstats(options, &appLog);

  PvlGroup group = appLog.findGroup("Results");
  EXPECT_EQ( (double) group.findKeyword("ThicknessMinimum"), 0.37266300670341);
  EXPECT_EQ( (double) group.findKeyword("ThicknessMaximum"), 0.37266300670341);
  EXPECT_EQ( (double) group.findKeyword("ThicknessAverage"), 0.37266300670341);
  EXPECT_EQ( (double) group.findKeyword("ThicknessStandardDeviation"), -1.79769313486231e+308);
  EXPECT_EQ( (double) group.findKeyword("ThicknessVariance"), -1.79769313486231e+308);
  EXPECT_EQ( (double) group.findKeyword("AreaMinimum"), 83798250265.466);
  EXPECT_EQ( (double) group.findKeyword("AreaMaximum"), 83798250265.466);
  EXPECT_EQ( (double) group.findKeyword("AreaAverage"), 83798250265.466);
  EXPECT_EQ( (double) group.findKeyword("AreaStandardDeviation"), -1.79769313486231e+308);
  EXPECT_EQ( (double) group.findKeyword("AreaVariance"), -1.79769313486231e+308);
  EXPECT_EQ( (double) group.findKeyword("ImageStackMinimum"), 2.0);
  EXPECT_EQ( (double) group.findKeyword("ImageStackMaximum"), 2.0);
  EXPECT_EQ( (double) group.findKeyword("ImageStackAverage"), 2.0);
  EXPECT_EQ( (double) group.findKeyword("ImageStackStandardDeviation"), -1.79769313486231e+308);
  EXPECT_EQ( (double) group.findKeyword("ImageStackVariance"), -1.79769313486231e+308);
  EXPECT_EQ( (int) group.findKeyword("PolygonCount"), 2);
}


TEST_F(ThreeImageNetwork, FunctionalTestOverlapstatsNoOverlap) {
  // create footprint for cube3 that has no overlap with cubes 1 and 2
  lonLatPts = new geos::geom::CoordinateArraySequence();
  lonLatPts->add(geos::geom::Coordinate(50, 50));
  lonLatPts->add(geos::geom::Coordinate(50, 40));
  lonLatPts->add(geos::geom::Coordinate(40, 40));
  lonLatPts->add(geos::geom::Coordinate(40, 50));
  lonLatPts->add(geos::geom::Coordinate(50, 50));

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
  cube3->write(pvlBlob);
  cube3->reopen("rw");

  FileList cubes;
  cubes.append(FileName(cube1->fileName()));
  cubes.append(FileName(cube2->fileName()));
  cubes.append(FileName(cube3->fileName()));
  cube1->close();
  cube2->close();
  cube3->close();

  QString cubeListPath = tempDir.path() + "/cubes.lis";
  QString overlapPath = tempDir.path() + "/overlaps.lis";
  cubes.write(cubeListPath);

  // get overlaps for overlapstats input
  QString FIO_APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();
  QVector<QString> fio_args = { "OVERLAPLIST=" + overlapPath };
  UserInterface fioOptions(FIO_APP_XML, fio_args);
  findimageoverlaps(cubes, fioOptions);

  QVector<QString> args = { "FROMLIST=" + cubeListPath,
                            "OVERLAPLIST=" + overlapPath };
  UserInterface options(APP_XML, args);
  Pvl appLog;
  overlapstats(options, &appLog);

  PvlGroup group = appLog.findGroup("Results");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, group.findKeyword("NoOverlap"), tempDir.path() + "/cube3.cub");
}
