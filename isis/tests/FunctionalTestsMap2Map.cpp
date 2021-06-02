#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "map2map.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/map2map.xml").expanded();


TEST_F(ThreeImageNetwork, FunctionalTestMap2mapDefault) {

  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from=" + cube1map->fileName(),
                           "to=" + outCubeFileName,
                           "map=" + cube2map->fileName(),
                           "defaultrange=map",
                           "INTERP=NEARESTNEIGHBOR"};

  UserInterface options(APP_XML, args);
  try {
    map2map(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  Cube ocube(outCubeFileName);

  PvlGroup mapping = ocube.label()->findGroup("Mapping", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, (QString)mapping.findKeyword("ProjectionName"), "Equirectangular");
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("CenterLongitude"), 0.25400668736684);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, (QString)mapping.findKeyword("TargetName"), "Mars");
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("EquatorialRadius"), 3396190.0);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("PolarRadius"), 3376200.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, (QString)mapping.findKeyword("LatitudeType"), "Planetocentric");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, (QString)mapping.findKeyword("LongitudeDirection"), "PositiveEast");
  EXPECT_EQ((int)mapping.findKeyword("LongitudeDomain"), 180);

  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("MinimumLatitude"), 0.47920860194551);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("MaximumLatitude"), 3.3932951263901);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("MinimumLongitude"), -0.94830771139743);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("MaximumLongitude"), 1.4318179715731);

  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("UpperLeftCornerX"), -71250.037709109);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("UpperLeftCornerY"), 201236.66564437);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("PixelResolution"), 255.37647924412);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("Scale"), 232.10614255659);
  EXPECT_DOUBLE_EQ((double)mapping.findKeyword("CenterLatitude"), 1.5210901942398);
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

  // EXPECT some stuff
}*/
