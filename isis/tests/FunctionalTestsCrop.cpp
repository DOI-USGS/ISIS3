#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "crop.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/crop.xml").expanded();

TEST_F(LargeCube, FunctionalTestCropDefault) {
  QTemporaryDir tempDir;
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
    "sample=818", "nsamples=100", "sinc=1", "line=700", "nlines=200", "linc=1"};

  UserInterface options(APP_XML, args);
  try {
    crop(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  Cube oCube(outCubeFileName, "r");

  Histogram *oCubeStats = oCube.histogram();

  EXPECT_NEAR(oCubeStats->Average(), 798.5, 0.01);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 15970000);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 20000);
  EXPECT_NEAR(oCubeStats->StandardDeviation(), 57.735748638374318, 0.0000000001);
}

TEST_F(DefaultCube, FunctionalTestCropSkip1) {
  QTemporaryDir tempDir;
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
    "sample=1", "nsamples=10", "sinc=1", "line=3", "nlines=1", "linc=1"};

  UserInterface options(APP_XML, args);
  try {
    crop(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
}

  Cube oCube(outCubeFileName, "r");

  Histogram *oCubeStats = oCube.histogram();

  EXPECT_DOUBLE_EQ(oCubeStats->Average(), 505.25);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 50525);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 100);
  EXPECT_DOUBLE_EQ(oCubeStats->StandardDeviation(), 302.16673352386897);
}

TEST_F(DefaultCube, FunctionalTestCropSkip2) {
  QTemporaryDir tempDir;
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
    "sample=50", "nsamples=50", "sinc=2", "line=50", "nlines=50", "linc=3"};

  UserInterface options(APP_XML, args);
  try {
    crop(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  Cube oCube(outCubeFileName, "r");

  Histogram *oCubeStats = oCube.histogram();

  EXPECT_DOUBLE_EQ(oCubeStats->Average(), 505.25);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 50525);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 100);
  EXPECT_DOUBLE_EQ(oCubeStats->StandardDeviation(), 302.16673352386897);
}
