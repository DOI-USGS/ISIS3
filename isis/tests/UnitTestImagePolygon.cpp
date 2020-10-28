#include "ImagePolygon.h"
#include "LineManager.h"
#include "PolygonTools.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/geom/CoordinateArraySequence.h"

#include <gtest/gtest.h>

#include "Fixtures.h"

using namespace Isis;

TEST(ImagePolygon, UnitTestImagePolygonDefaultParams) {
  //   simple MOC image
  QString inFile = "$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.cub";

  // Open the cube
  Cube cube;
  cube.open(inFile, "r");

  ImagePolygon poly;
  try {
    poly.Create(cube, 1000, 1000);
  }
  catch(IException &e) {
    QString msg = "Cannot create polygon for [" + cube.fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  std::vector<double> lons = {223.817465, 233.818051, 236.053039, 219.688243, 223.817465};
  std::vector<double> lats = {34.457676, 33.787158, 10.767127, 11.745635, 34.457676};

  for (auto poly : *(poly.Polys())) {
    geos::geom::CoordinateArraySequence coordArray = geos::geom::CoordinateArraySequence(*(poly->getCoordinates()));
    for (int i = 0; i < coordArray.getSize(); i++) {
      EXPECT_NEAR(lons[i], coordArray.getAt(i).x, 1e-6);
      EXPECT_NEAR(lats[i], coordArray.getAt(i).y, 1e-6);
    }
  }

  cube.close();
}

TEST(ImagePolygon, UnitTestImagePolygonSubPoly) {
  //   simple MOC image
  QString inFile = "$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.cub";

  // Open the cube
  Cube cube;
  cube.open(inFile, "r");

  ImagePolygon poly;
  try {
    poly.Create(cube, 100, 100, 384, 640, 385);
  }
  catch(IException &e) {
    QString msg = "Cannot create polygon for [" + cube.fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  std::vector<double> lons = {225.657872, 227.226450, 230.300664, 234.551660,
                              235.675005, 235.991430, 228.952508, 226.483092,
                              225.530322, 225.638213, 225.657872};
  std::vector<double> lats = {15.202335, 15.113528, 14.906305, 14.543820,
                              11.594707, 10.770232, 11.308501, 11.457097,
                              11.514591, 14.378682, 15.202335};

  for (auto poly : *(poly.Polys())) {
    geos::geom::CoordinateArraySequence coordArray = geos::geom::CoordinateArraySequence(*(poly->getCoordinates()));
    for (int i = 0; i < coordArray.getSize(); i++) {
      EXPECT_NEAR(lons[i], coordArray.getAt(i).x, 1e-6);
      EXPECT_NEAR(lats[i], coordArray.getAt(i).y, 1e-6);
    }
  }

  cube.close();
}

TEST_F(TempTestingFiles, UnitTestImagePolygonCross) {
  FileName isdFile("$ISISROOT/../isis/tests/data/footprintinit/cross.isd");
  FileName labelFile("$ISISROOT/../isis/tests/data/footprintinit/cross.pvl");

  Cube crossCube;
  crossCube.fromIsd(tempDir.path() + "footprintCube.cub", labelFile, isdFile, "rw");

  ImagePolygon poly;
  try {
    poly.Create(crossCube, 100, 100);
  }
  catch(IException &e) {
    QString msg = "Cannot create polygon for [" + crossCube.fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  std::vector<double> lons = {0.000000, 12.467474, 21.544208, 27.693862,
                              32.115185, 35.471466, 38.127194, 40.290139,
                              40.345196, 37.640935, 34.997561, 32.400527,
                              29.817102, 27.267075, 24.742958, 22.241039,
                              19.755910, 17.283211, 14.791622, 13.394818,
                              9.905657, 5.816117, 0.912785, 0.000000, 0.000000,
                              0.000000, 286.909525, 333.822499, 357.984300,
                              360.000000, 360.000000, 360.000000, 354.859847,
                              347.059511, 336.340528, 317.180139, 308.824543,
                              307.775655, 305.511096, 304.451965, 302.574302,
                              300.053776, 298.105128, 295.343612, 293.012482,
                              289.800680, 286.909525};
  std::vector<double> lats = {76.431862, 74.583621, 72.204590, 69.854646,
                              67.580179, 65.391246, 63.285144, 61.263799,
                              61.191906, 60.607862, 60.004081, 59.382798,
                              58.753608, 58.125676, 57.465212, 56.781693,
                              56.080041, 55.364106, 54.625251, 54.208706,
                              55.744385, 57.248327, 58.695391, 58.897164,
                              71.056008, 76.431862, 72.459546, 77.858556,
                              76.730680, 76.431862, 71.056008, 58.897164,
                              60.033405, 61.142862, 61.738642, 60.284299,
                              60.256669, 61.708251, 62.900241, 64.355582,
                              65.678283, 66.807752, 68.054877, 69.175840,
                              70.327896, 71.379534, 72.459546};

  geos::geom::CoordinateArraySequence outputCoordArray = geos::geom::CoordinateArraySequence();

  for (auto poly : *(poly.Polys())) {
    geos::geom::CoordinateArraySequence coordArray = geos::geom::CoordinateArraySequence(*(poly->getCoordinates()));
    for (int i = 0; i < coordArray.getSize(); i++) {
      outputCoordArray.add(coordArray.getAt(i));
    }
  }

  for (int i = 0; i < outputCoordArray.getSize(); i++) {
    EXPECT_NEAR(lons[i], outputCoordArray.getAt(i).x, 1e-6);
    EXPECT_NEAR(lats[i], outputCoordArray.getAt(i).y, 1e-6);
  }
}

TEST_F(DefaultCube, UnitTestImagePolygonBoundary) {
  Pvl footprintLabel;

  std::ifstream isdFile("data/footprintinit/boundary.isd");
  std::ifstream cubeLabel("data/footprintinit/MessengerInstrument.pvl");

  isdFile >> isd;
  cubeLabel >> footprintLabel;

  PvlObject core = footprintLabel.findObject("IsisCube").findObject("Core");
  PvlGroup instrument = footprintLabel.findObject("IsisCube").findGroup("Instrument");

  label.findObject("IsisCube").deleteObject("Core");
  label.findObject("IsisCube").addObject(core);
  label.findObject("IsisCube").deleteGroup("Instrument");
  label.findObject("IsisCube").addGroup(instrument);

  label.findObject("IsisCube").findGroup("Kernels")["CameraVersion"] = "2";
  label.findObject("IsisCube").findGroup("Kernels")["NaifFrameCode"] = "-236820";
  label.findObject("IsisCube").findGroup("Kernels")["SpacecraftClock"] = "$messenger/kernels/sclk/messenger_2548.tsc";
  label.findObject("IsisCube").findGroup("Kernels")["ShapeModel"] = "$base/dems/MSGR_DEM_USG_EQ_I_V02_prep.cub";

  Cube footprintCube;
  footprintCube.fromIsd(tempDir.path() + "footprintCube.cub", label, isd, "rw");

  ImagePolygon poly;
  try {
    poly.Create(footprintCube, 3000, 3000);
  }
  catch(IException &e) {
    QString msg = "Cannot create polygon for [" + footprintCube.fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  std::vector<double> lons = {222.276316, 222.252869, 257.336775, 262.514561,
                              236.625120, 222.276316};
  std::vector<double> lats = {19.812114, 26.058469, 25.620629, 13.155262,
                              12.939325, 19.812114};

  for (auto poly : *(poly.Polys())) {
    geos::geom::CoordinateArraySequence coordArray = geos::geom::CoordinateArraySequence(*(poly->getCoordinates()));
    for (int i = 0; i < coordArray.getSize(); i++) {
      EXPECT_NEAR(lons[i], coordArray.getAt(i).x, 1e-6);
      EXPECT_NEAR(lats[i], coordArray.getAt(i).y, 1e-6);
    }
  }

  footprintCube.close();
}

TEST_F(TempTestingFiles, UnitTestImagePolygonMosaic) {
  Pvl footprintLabel;
  std::ifstream cubeLabel("data/footprintinit/mosaic.pvl");

  cubeLabel >> footprintLabel;

  Cube footprintCube;
  FileName footprintCube2(tempDir.path() + "footprintCube.cub");
  footprintCube.fromLabel(footprintCube2, footprintLabel, "rw");

  LineManager line(footprintCube);
  double pixelValue = 1.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      line[i] = (double) pixelValue;
    }
    footprintCube.write(line);
  }

  ImagePolygon poly;
  try {
    poly.Create(footprintCube, 3000, 3000);
  }
  catch(IException &e) {
    QString msg = "Cannot create polygon for [" + footprintCube.fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  std::vector<double> lons = {347.895055, 349.247803, 349.699169, 349.699395,
                              349.699244, 348.346422, 347.895055, 347.895055,
                              347.895055};
  std::vector<double> lats = {-42.323638, -42.323638, -42.323638, -43.312974,
                              -43.643248, -43.643248, -43.643083, -42.653746,
                              -42.323638};

  for (auto poly : *(poly.Polys())) {
    geos::geom::CoordinateArraySequence coordArray = geos::geom::CoordinateArraySequence(*(poly->getCoordinates()));
    for (int i = 0; i < coordArray.getSize(); i++) {
      EXPECT_NEAR(lons[i], coordArray.getAt(i).x, 1e-6);
      EXPECT_NEAR(lats[i], coordArray.getAt(i).y, 1e-6);
    }
  }

  footprintCube.close();
}

TEST_F(DefaultCube, UnitTestImagePolygonOutlier) {
  Pvl footprintLabel;

  std::ifstream isdFile("data/footprintinit/outlier.isd");
  std::ifstream cubeLabel("data/footprintinit/MessengerInstrument.pvl");

  isdFile >> isd;
  cubeLabel >> footprintLabel;

  PvlObject core = footprintLabel.findObject("IsisCube").findObject("Core");
  PvlGroup instrument = footprintLabel.findObject("IsisCube").findGroup("Instrument");

  instrument["SpacecraftClockCount"] = "1/0215651170:929000";

  PvlObject isisCube = label.findObject("IsisCube");

  isisCube.deleteObject("Core");
  isisCube.addObject(core);
  isisCube.deleteGroup("Instrument");
  isisCube.addGroup(instrument);

  PvlKeyword number("Number", "9");
  isisCube.findGroup("BandBin").addKeyword(number);

  isisCube.findGroup("Kernels")["CameraVersion"] = "2";
  isisCube.findGroup("Kernels")["NaifFrameCode"] = "-236800";
  isisCube.findGroup("Kernels")["SpacecraftClock"] = "$messenger/kernels/sclk/messenger_2548.tsc";
  isisCube.findGroup("Kernels")["ShapeModel"] = "Null";

  label.deleteObject("IsisCube");
  label.addObject(isisCube);

  Cube footprintCube;
  // footprintCube.fromIsd(tempDir.path() + "footprintCube.cub", label, isd, "rw");
  footprintCube.fromIsd("/Users/acpaquette/Desktop/footprintCube.cub", label, isd, "rw");

  ImagePolygon poly;
  poly.Emission(89);
  poly.Incidence(89);
  try {
    poly.Create(footprintCube, 200, 200);
  }
  catch(IException &e) {
    std::cout << e.what() << '\n';
    QString msg = "Cannot create polygon for [" + footprintCube.fileName() + "]";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  std::vector<double> lons = {269.520359, 269.450334, 269.116332, 268.488713,
                              268.081499, 236.714330, 213.246253, 203.074333,
                              208.633919, 224.744265, 242.007758, 256.524583,
                              269.520359};
  std::vector<double> lats = {5.845377, -32.123188, -48.643335, -62.120850,
                              -66.796093, -62.950610, -51.024263, -39.134800,
                              -15.146478, -9.173929, -2.744727, 2.159646,
                              5.845377};

  for (auto poly : *(poly.Polys())) {
    geos::geom::CoordinateArraySequence coordArray = geos::geom::CoordinateArraySequence(*(poly->getCoordinates()));
    for (int i = 0; i < coordArray.getSize(); i++) {
      EXPECT_NEAR(lons[i], coordArray.getAt(i).x, 1e-6);
      EXPECT_NEAR(lats[i], coordArray.getAt(i).y, 1e-6);
    }
  }

  footprintCube.close();
}
