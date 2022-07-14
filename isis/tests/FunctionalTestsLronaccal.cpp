#include <QTemporaryDir>
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Endian.h"
#include "PixelType.h"
#include "lronaccal.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Histogram.h"
#include "crop.h"

using namespace std;
using namespace Isis;
using namespace testing; 

/**
  *
  * @brief Calibration application for the LRO NAC cameras
  *
  *  lronaccal performs radiometric corrections to images acquired by the Narrow Angle
  *  Camera aboard the Lunar Reconnaissance Orbiter spacecraft.
  *
  * @author 2016-09-16 Victor Silva
  *
  * @internal
  *   @history 2022-04-26 Victor Silva - Original Version - Functional test is against known value for input 
  *                                      cub since fx is not yet callable
  */

static QString APP_XML = FileName("$ISISROOT/bin/xml/lronaccal.xml").expanded();

TEST(LronaccalDefault, FunctionalTestsLronaccal) {
  QTemporaryDir outputDir;

  ASSERT_TRUE(outputDir.isValid());

  /*This application can not be run on any image that has been
    geometrically transformed (i.e. scaled, rotated, sheared, or
    reflected) or cropped
  */
  QString iCubeFile =  "data/lronaccal/input/M1333276014R.cub";
  QString oCubeFile = outputDir.path() + "/out.default.cub";
  QString oCubeCropFile = outputDir.path() + "/out.default.crop.cub";
  QString tCubeFile = "data/lronaccal/truth/M1333276014R.default.crop.cub";

  QVector<QString> args = {"from="+iCubeFile, "to="+oCubeFile }; 
 
  UserInterface options(APP_XML, args);
  try {
    lronaccal(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to calibrate the LRO image: " <<e.toString().toStdString().c_str() << std::endl;
  }
  try{
    static QString CROP_XML = FileName("$ISISROOT/bin/xml/crop.xml").expanded();
    QVector<QString> argsCrop = {"from=" + oCubeFile, "to=" + oCubeCropFile, "sample=80", "nsamples=80", "line=80", "nlines=80"};
    UserInterface options2(CROP_XML, argsCrop);
    crop(options2);
    
    Cube oCube(oCubeCropFile, "r");
    Cube tCube(tCubeFile, "r");

    Histogram *oCubeStats = oCube.histogram();
    Histogram *tCubeStats = tCube.histogram();

    EXPECT_NEAR(oCubeStats->Average(), tCubeStats->Average(), 0.001);
    EXPECT_NEAR(oCubeStats->Sum(), tCubeStats->Sum(), 0.001);
    EXPECT_NEAR(oCubeStats->ValidPixels(), tCubeStats->ValidPixels(), 0.001);
    EXPECT_NEAR(oCubeStats->StandardDeviation(), tCubeStats->StandardDeviation(), 0.001);
    
    tCube.close();
    oCube.close();
  }
  catch(IException &e){
    FAIL() << "Unable to compare stats: " <<e.toString().toStdString().c_str() << std::endl;
  }
}

TEST(LronaccalNear, FunctionalTestsLronaccal) {
  QTemporaryDir outputDir;

  ASSERT_TRUE(outputDir.isValid());

  /*This application can not be run on any image that has been
    geometrically transformed (i.e. scaled, rotated, sheared, or
    reflected) or cropped
  */
  QString iCubeFile =  "data/lronaccal/input/M1333276014R.cub";
  QString oCubeFile = outputDir.path() + "/out.near.cub";
  QString oCubeCropFile = outputDir.path() + "/out.near.crop.cub";
  QString tCubeFile = "data/lronaccal/truth/M1333276014R.near.crop.cub";

  QVector<QString> args = {"from="+iCubeFile, "to="+oCubeFile, "DarkFileType=NEAREST"};
 
  UserInterface options(APP_XML, args);
  try {
    lronaccal(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to calibrate the LRO image: " <<e.toString().toStdString().c_str() << std::endl;
  }
  try{  
    static QString CROP_XML = FileName("$ISISROOT/bin/xml/crop.xml").expanded();
    QVector<QString> argsCrop = {"from=" + oCubeFile, "to=" + oCubeCropFile, "sample=80", "nsamples=80", "line=80", "nlines=80"};
    UserInterface options2(CROP_XML, argsCrop);
    crop(options2);
    
    Cube oCube(oCubeCropFile, "r");
    Cube tCube(tCubeFile, "r");

    Histogram *oCubeStats = oCube.histogram();
    Histogram *tCubeStats = tCube.histogram();

    EXPECT_NEAR(oCubeStats->Average(), tCubeStats->Average(), 0.001);
    EXPECT_NEAR(oCubeStats->Sum(), tCubeStats->Sum(), 0.001);
    EXPECT_NEAR(oCubeStats->ValidPixels(), tCubeStats->ValidPixels(), 0.001);
    EXPECT_NEAR(oCubeStats->StandardDeviation(), tCubeStats->StandardDeviation(), 0.001);
    
    tCube.close();
    oCube.close();
  }
  catch(IException &e){
    FAIL() << "Unable to open output cube: " <<e.toString().toStdString().c_str() << std::endl;
  }
}

TEST(LronaccalPair, FunctionalTestsLronaccal) {
  QTemporaryDir outputDir;

  ASSERT_TRUE(outputDir.isValid());

  QString iCubeFile =  "data/lronaccal/input/M1333276014R.cub";
  QString oCubeFile = outputDir.path() + "/out.pair.cub";
  QString oCubeCropFile = outputDir.path() + "/out.pair.crop.cub";
  QString tCubeFile = "data/lronaccal/truth/M1333276014R.pair.crop.cub";

  QVector<QString> args = {"from="+iCubeFile, "to="+oCubeFile, "DarkFileType=PAIR"};
 
  UserInterface options(APP_XML, args);
  try {
    lronaccal(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to calibrate the LRO image: " <<e.toString().toStdString().c_str() << std::endl;
  }
  try{
    static QString CROP_XML = FileName("$ISISROOT/bin/xml/crop.xml").expanded();
    QVector<QString> argsCrop = {"from=" + oCubeFile, "to=" + oCubeCropFile, "sample=80", "nsamples=80", "line=80", "nlines=80"};
    UserInterface options2(CROP_XML, argsCrop);
    crop(options2);
    
    Cube oCube(oCubeCropFile, "r");
    Cube tCube(tCubeFile, "r");

    Histogram *oCubeStats = oCube.histogram();
    Histogram *tCubeStats = tCube.histogram();

    EXPECT_NEAR(oCubeStats->Average(), tCubeStats->Average(), 0.001);
    EXPECT_NEAR(oCubeStats->Sum(), tCubeStats->Sum(), 0.001);
    EXPECT_NEAR(oCubeStats->ValidPixels(), tCubeStats->ValidPixels(), 0.001);
    EXPECT_NEAR(oCubeStats->StandardDeviation(), tCubeStats->StandardDeviation(), 0.001);
    
    tCube.close();
    oCube.close();
  }
  catch(IException &e){
    FAIL() << "Unable to open output cube: " <<e.toString().toStdString().c_str() << std::endl;
  }
}

