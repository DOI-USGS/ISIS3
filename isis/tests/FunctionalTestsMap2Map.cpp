#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "map2map.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/map2map.xml").expanded();


TEST_F(DefaultCube, FunctionalTestMap2MapDefault) {
  // tempDir exists if the fixture subclasses TempTestingFiles, which most do
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
    "defaultrange=map", "INTERP=NEARESTNEIGHBOR"};

  UserInterface options(APP_XML, args);
  Pvl appLog;
  try {
    //map2map(options, &appLog);
    map2map(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  /*Pvl pvlobject = Pvl(outCubeFileName);

  ASSERT_TRUE(pvlobject.hasObject("Mapping"));
  PvlObject mapobj = pvlobject.findObject("Mapping");*/

  Cube oCube(outCubeFileName, "r");


  Histogram *oCubeStats = oCube.histogram();

  EXPECT_NEAR(oCubeStats->Average(), 73, 0.01);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 31025);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 425);
  EXPECT_NEAR(oCubeStats->StandardDeviation(), 14.714259545157688, 0.0000000001);


  //ASSERT_TRUE(pvlobject.hasObject("UserParameters"));
  //PvlObject userparameters = pvlObject.findObject("UserParameters");
  //EXPECT_EQ(userparameters.findKeyword("INTERP"), "NEARESTNEIGHBOR");
  /*ASSERT_TRUE(pvlobj.hasObject("Camstats"));
  PvlObject camstats = camobj.findObject("Camstats");*/

  // check statistics
  // pick lat/lon in input, and check pixel value for input and output are close
  // should account for diff projection, so only diff should be interpolation error, which is small

  // Assert some stuff
}

/*
TEST_F(DefaultCube, FunctionalTestMap2MapKeywords) {

    // check scale value in mapping label and the size
    // smaller scale = larger image

  // tempDir exists if the fixture subclasses TempTestingFiles, which most do
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
    "MATCHMAP=yes"};

  UserInterface options(APP_XML, args);
  Pvl appLog;
  try {
    map2map(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  // make sure stuff in log matches the input
  // make sure the other value defaults are fine
    // interp
}

TEST_F(DefaultCube, FunctionalTestMap2MapUnits) {
  // tempDir exists if the fixture subclasses TempTestingFiles, which most do
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
    "MAXLONG=-109"};

  UserInterface options(APP_XML, args);
  Pvl appLog;
  try {
    map2map(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  // Assert some stuff
}*/
