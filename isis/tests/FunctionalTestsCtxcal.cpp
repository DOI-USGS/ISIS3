#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "ctxcal.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/ctxcal.xml").expanded();


TEST_F(MroCtxCube, FunctionalTestCtxcalDefault) {
  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path() + "/outTemp.cub";
  QVector<QString> args = {"to="+outCubeFileName};

  UserInterface options(APP_XML, args);
  
  try {
    ctxcal(testCube.get(), options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  Cube oCube(outCubeFileName, "r");

  Histogram *oCubeStats = oCube.histogram();

  EXPECT_DOUBLE_EQ(oCubeStats->Average(), 0.077640061192214491);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 31.056024476885796);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 400);
  EXPECT_DOUBLE_EQ(oCubeStats->StandardDeviation(), 0.0012347471238170408);
}


TEST_F(MroCtxCube, FunctionalTestCtxcalFlatfile) {
  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path() + "/outTemp.cub";
  QVector<QString> args = {"to="+outCubeFileName, "flatfile=$ISISDATA/mro/calibration/ctxFlat_0001.cub"};

  UserInterface options(APP_XML, args);
  
  try {
    ctxcal(testCube.get(), options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  Cube oCube(outCubeFileName, "r");

  Histogram *oCubeStats = oCube.histogram();

  EXPECT_DOUBLE_EQ(oCubeStats->Average(), 0.10046864503994585);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 40.187458015978336);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 400);
  EXPECT_DOUBLE_EQ(oCubeStats->StandardDeviation(), 0.0018248585597074806);
}


TEST_F(MroCtxCube, FunctionalTestCtxcalIofFalse) {
  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path() + "/outTemp.cub";
  QVector<QString> args = {"to="+outCubeFileName, "iof=false"};

  UserInterface options(APP_XML, args);
  
  try {
    ctxcal(testCube.get(), options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  Cube oCube(outCubeFileName, "r");

  Histogram *oCubeStats = oCube.histogram();

  EXPECT_DOUBLE_EQ(oCubeStats->Average(), 221.12296661376953);
  EXPECT_DOUBLE_EQ(oCubeStats->Sum(), 88449.186645507812);
  EXPECT_DOUBLE_EQ(oCubeStats->ValidPixels(), 400);
  EXPECT_DOUBLE_EQ(oCubeStats->StandardDeviation(), 3.5166241557192071);
}


TEST_F(MroCtxCube, FunctionalTestCtxcalCameraComparison) {
  QTemporaryDir prefix;
  QString outCubeFileNameCam = prefix.path() + "/outTemp.cub";
  QVector<QString> args = {"to="+outCubeFileNameCam};

  UserInterface options(APP_XML, args);
  
  try {
    ctxcal(testCube.get(), options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  
  // force camera to not construct
  Pvl *lab = testCube->label(); 
  lab->deleteObject("NaifKeywords");

  QString outCubeFileNameNoCam = prefix.path() + "/outTemp.cub";
  args = {"to="+outCubeFileNameNoCam};
  
  try {
    ctxcal(testCube.get(), options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  
  Cube oNoCamCube(outCubeFileNameCam, "r");
  Cube oCamCube(outCubeFileNameCam, "r");

  Pvl *noCamLab = oNoCamCube.label(); 
  Pvl *camLab = oCamCube.label();

  EXPECT_DOUBLE_EQ((double)noCamLab->findObject("IsisCube").findGroup("Radiometry").findKeyword("iof"), 
                   (double)camLab->findObject("IsisCube").findGroup("Radiometry").findKeyword("iof"));
  
}

