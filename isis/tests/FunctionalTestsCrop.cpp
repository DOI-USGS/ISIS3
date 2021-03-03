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

TEST_F(LargeCube, FunctionalTestCropSkip1) {
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

  EXPECT_NEAR(oCubeStats->Average(), 2, 0.01);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 20);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 10);
  EXPECT_NEAR(oCubeStats->StandardDeviation(), 0, 0.0000000001);
}

TEST_F(LargeCube, FunctionalTestCropSkip2) {
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

  EXPECT_NEAR(oCubeStats->Average(), 73, 0.01);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 31025);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 425);
  EXPECT_NEAR(oCubeStats->StandardDeviation(), 14.714259545157688, 0.0000000001);
}
