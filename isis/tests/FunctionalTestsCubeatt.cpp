#include <iostream>

#include "Cube.h"
#include "Fixtures.h"

#include "cubeatt.h"
#include "gmock/gmock.h"


using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cubeatt.xml").expanded();

// Tests setting output attributes: bit type and range
TEST_F(SmallCube, FunctionalTestCubeattBitttypeAndRange) {
  QString cubePath = "/scratch/without.cub+8bit+0.0:1.0";
  QVector<QString> args = {"from=" + testCube->fileName(), "to=" + cubePath};
  UserInterface options(APP_XML, args);
  cubeatt(options);

  // Test the label
  Cube outputCube(cubePath);
  Pvl *label = outputCube.label();
  PvlGroup &pixelGroup = label->findObject("IsisCube").findObject("Core").findGroup("Pixels");
  EXPECT_EQ(QString(pixelGroup["Type"]).toStdString(), "UnsignedByte");
  EXPECT_NEAR(pixelGroup["Base"][0].toDouble(), -0.003952569, 0.00001);
  EXPECT_NEAR(pixelGroup["Multiplier"][0].toDouble(), 0.003952569, 0.00001);

  // Test the DNs
}

TEST_F(SmallCube, FunctionalTestCubeattNoChange) {
  QString cubePath = "/scratch/without.cub";
  QVector<QString> args = {"from=" + testCube->fileName() + "+1", "to=" + cubePath};
  UserInterface options(APP_XML, args);
  cubeatt(options);

  // Test the label
  Cube outputCube(cubePath);
  Pvl *label = outputCube.label();
  PvlGroup &pixelGroup = label->findObject("IsisCube").findObject("Core").findGroup("Pixels");
  EXPECT_EQ(QString(pixelGroup["Type"]).toStdString(), "Real");
  EXPECT_EQ(pixelGroup["Base"][0].toDouble(), 0);
  EXPECT_EQ(pixelGroup["Multiplier"][0].toDouble(), 1);

  // Test that DNs match in the input and output cubes
}



TEST_F(SmallCube, FunctionalTestCubeattVirtualBands) {
  QString cubePath = "/scratch/bands.cub";
  QVector<QString> args = {"from=" + testCube->fileName() + "+3,2,4,2,1,5,7,6,4", "to=" + cubePath};
  UserInterface options(APP_XML, args);
  cubeatt(options);

  // Test the label
  Cube outputCube(cubePath);
  Pvl *label = outputCube.label();
  PvlGroup &dimensionsGroup = label->findObject("IsisCube").findObject("Core").findGroup("Dimensions");

  // Test the label
  EXPECT_EQ(QString(dimensionsGroup["Bands"]).toInt(), 9);

  // Test DNs
  // Test to see if bands were moved around as appropriate
}


