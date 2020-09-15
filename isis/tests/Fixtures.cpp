#include "Fixtures.h"
#include "LineManager.h"
#include "SpecialPixel.h"

namespace Isis {

  void TempTestingFiles::SetUp() {
    ASSERT_TRUE(tempDir.isValid());
  }


  void SmallCube::SetUp() {
    TempTestingFiles::SetUp();

    testCube = new Cube();
    testCube->setDimensions(10, 10, 10);
    testCube->create(tempDir.path() + "/small.cub");

    LineManager line(*testCube);
    double pixelValue = 0.0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) pixelValue++;
      }
      testCube->write(line);
    }

  }

  void SmallCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (testCube) {
      delete testCube;
    }
  }

  void SpecialSmallCube::SetUp() {
    TempTestingFiles::SetUp();

    testCube = new Cube();
    testCube->setDimensions(10, 10, 10);
    testCube->create(tempDir.path() + "/small.cub");

    // Use a line manager to update select lines with ISIS special pixel values
    LineManager line(*testCube);
    double pixelValue = 0.0;
    int lineNum = 0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        if (lineNum == 2) {
          line[i] = NULL8;
        }
        else if (lineNum == 3) {
          line[i] = LOW_REPR_SAT8;
        }
        else if (lineNum == 4) {
          line[i] = HIGH_REPR_SAT8;
        }
        else if (lineNum == 5) {
          line[i] = LOW_INSTR_SAT8;
        }
        else if (lineNum == 6) {
          line[i] = HIGH_INSTR_SAT8;
        }
        else {
          line[i] = (double) pixelValue++;
        }
      }
      lineNum++;
      testCube->write(line);
    }

  }

  void SpecialSmallCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (testCube) {
      delete testCube;
    }
  }


  void DefaultCube::SetUp() {
    TempTestingFiles::SetUp();

    std::ifstream isdFile("data/defaultImage/defaultCube.isd");
    std::ifstream cubeLabel("data/defaultImage/defaultCube.pvl");
    std::ifstream projCubeLabel("data/defaultImage/projDefaultCube.pvl");

    isdFile >> isd;
    cubeLabel >> label;
    projCubeLabel >> projLabel;

    testCube = new Cube();
    testCube->fromIsd(tempDir.path() + "/default.cub", label, isd, "rw");

    projTestCube = new Cube();
    projTestCube->fromIsd(tempDir.path() + "/default.level2.cub", projLabel, isd, "rw");
  }


  void DefaultCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (projTestCube->isOpen()) {
      projTestCube->close();
    }

    delete testCube;
    delete projTestCube;
  }

  void LineScannerCube::SetUp() {
    TempTestingFiles::SetUp();

    std::ifstream isdFile("data/LineScannerImage/defaultLineScanner.isd");
    std::ifstream cubeLabel("data/LineScannerImage/defaultLineScanner.pvl");
    std::ifstream projCubeLabel("data/LineScannerImage/projDefaultLineScanner.pvl");

    isdFile >> isd;
    cubeLabel >> label;
    projCubeLabel >> projLabel;

    testCube = new Cube();
    testCube->fromIsd(tempDir.path() + "/default.cub", label, isd, "rw");

    projTestCube = new Cube();
    projTestCube->fromIsd(tempDir.path() + "/default.level2.cub", projLabel, isd, "rw");
  }


  void LineScannerCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (projTestCube->isOpen()) {
      projTestCube->close();
    }

    delete testCube;
    delete projTestCube;
  }


  void ThreeImageNetwork::SetUp() {
    TempTestingFiles::SetUp();

    FileName labelPath1("data/threeImageNetwork/cube1.pvl");
    FileName labelPath2("data/threeImageNetwork/cube2.pvl");
    FileName labelPath3("data/threeImageNetwork/cube3.pvl");

    isdPath1 = new FileName("data/threeImageNetwork/cube1.isd");
    isdPath2 = new FileName("data/threeImageNetwork/cube2.isd");
    isdPath3 = new FileName("data/threeImageNetwork/cube3.isd");

    threeImageOverlapFile = new FileName("data/threeImageNetwork/threeImageOverlaps.lis");
    twoImageOverlapFile = new FileName("data/threeImageNetwork/twoImageOverlaps.lis");

    cube1 = new Cube();
    cube1->fromIsd(tempDir.path() + "/cube1.cub", labelPath1, *isdPath1, "rw");

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
    cube1->write(pvlBlob);
    cube1->reopen("rw");

    cube2 = new Cube();
    cube2->fromIsd(tempDir.path() + "/cube2.cub", labelPath2, *isdPath2, "rw");

    lonLatPts = new geos::geom::CoordinateArraySequence();
    lonLatPts->add(geos::geom::Coordinate(31, 1));
    lonLatPts->add(geos::geom::Coordinate(31, 11));
    lonLatPts->add(geos::geom::Coordinate(36, 11));
    lonLatPts->add(geos::geom::Coordinate(36, 1));
    lonLatPts->add(geos::geom::Coordinate(31, 1));

    polys->pop_back();
    poly = globalFactory->createPolygon(globalFactory->createLinearRing(lonLatPts), nullptr);
    polys->push_back(poly);
    multiPoly = globalFactory->createMultiPolygon(polys);

    polyStr = wkt->write(multiPoly);
    polyStrSize = polyStr.size();
    polyStream.str(polyStr);

    pvlBlob = Blob("Footprint", "Polygon");
    polyObject.addKeyword(PvlKeyword("Bytes", toString(polyStrSize)));
    pvl.addObject(polyObject);

    pvlBlob.Read(pvl, polyStream);
    cube2->write(pvlBlob);
    cube2->reopen("rw");

    delete wkt;

    cube3 = new Cube();
    cube3->fromIsd(tempDir.path() + "/cube3.cub", labelPath3, *isdPath3, "rw");

    cubeList = new FileList();
    cubeList->append(cube1->fileName());
    cubeList->append(cube2->fileName());
    cubeList->append(cube3->fileName());

    cubeListFile = tempDir.path() + "/cubes.lis";
    cubeList->write(cubeListFile);

    network = new ControlNet();
    network->ReadControl("data/threeImageNetwork/controlnetwork.net");
  }


  void ThreeImageNetwork::TearDown() {
    delete cubeList;
    delete network;

    delete cube1;
    delete cube2;
    delete cube3;

    delete isdPath1;
    delete isdPath2;
    delete isdPath3;

    delete threeImageOverlapFile;
    delete twoImageOverlapFile;
  }
}
