#include <iostream>

#include "Cube.h"
#include "Fixtures.h"

#include "cubeatt.h"
#include "gmock/gmock.h"


using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cubeatt.xml").expanded();

// Tests setting output attributes: bit type and range
TEST_F(SmallCube, FunctionalTestCubeattBitttypeAndRange) {
  QString cubePath = tempDir.path() + "/bitTypeCubeatt.cub+8bit+0.0:1.0";

  QVector<QString> args = {"from=" + testCube->fileName(), "to=" + cubePath};
  UserInterface options(APP_XML, args);
  cubeatt(options);

  Cube outputCube(cubePath);

  // Check attributes: pixel type, storage format, label format, storage order, pixel range, bands
  EXPECT_EQ(outputCube.pixelType(), PixelType::UnsignedByte);
  EXPECT_EQ(outputCube.format(), Cube::Format::Tile);
  EXPECT_TRUE(outputCube.labelsAttached());
  EXPECT_EQ(outputCube.byteOrder(), ByteOrder::Lsb);
  // Setting the pixel range modifies the base/multiplier, so check those.
  EXPECT_NE(outputCube.base(), 0);
  EXPECT_NE(outputCube.multiplier(), 1);
  EXPECT_EQ(outputCube.bandCount(), 10);  
  Pvl *label = outputCube.label();

  // Test the DNs
}

TEST_F(SmallCube, FunctionalTestCubeattNoChange) {
  QString cubePath = tempDir.path() + "/NoChangeCubeatt.cub";
  QVector<QString> args = {"from=" + testCube->fileName(), "to=" + cubePath};
  UserInterface options(APP_XML, args);
  cubeatt(options);

  Cube outputCube(cubePath);

  // Check attributes: pixel type, storage format, label format, storage order, pixel range, bands
  EXPECT_EQ(outputCube.pixelType(), PixelType::Real);
  EXPECT_EQ(outputCube.format(), Cube::Format::Tile);
  EXPECT_TRUE(outputCube.labelsAttached());
  EXPECT_EQ(outputCube.byteOrder(), ByteOrder::Lsb);
  // Setting the pixel range modifies the base/multiplier, so check those.
  EXPECT_EQ(outputCube.base(), 0);
  EXPECT_EQ(outputCube.multiplier(), 1);
  EXPECT_EQ(outputCube.bandCount(), 10);  
 
  // Test that DNs match in the input and output cubes
}



TEST_F(SmallCube, FunctionalTestCubeattVirtualBands) {
  QString cubePath = tempDir.path() + "/VirtualBandsCubeatt.cub";

  QVector<QString> args = {"from=" + testCube->fileName() + "+3,2,4,2,1,5,7,6,4", "to=" + cubePath};
  UserInterface options(APP_XML, args);
  cubeatt(options);

  Cube outputCube(cubePath);
  // Check attributes: pixel type, storage format, label format, storage order, pixel range, bands
  EXPECT_EQ(outputCube.pixelType(), PixelType::Real);
  EXPECT_EQ(outputCube.format(), Cube::Format::Tile);
  EXPECT_TRUE(outputCube.labelsAttached());
  EXPECT_EQ(outputCube.byteOrder(), ByteOrder::Lsb);
  // Setting the pixel range modifies the base/multiplier, so check those.
  EXPECT_EQ(outputCube.base(), 0);
  EXPECT_EQ(outputCube.multiplier(), 1);
  EXPECT_EQ(outputCube.bandCount(), 9);  

  // Test DNs
  // Test to see if bands were moved around as appropriate
}


