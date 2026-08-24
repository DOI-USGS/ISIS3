#include <iostream>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "demprep.h"

#include "Cube.h"
#include "Pvl.h"
#include "TempFixtures.h"
#include "TestUtilities.h"
#include "FileName.h"
#include "LineManager.h"
#include "Table.h"
#include "Histogram.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/demprep.xml").expanded();

TEST(Demprep, DemprepDefault){
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/padded.cub";
  QVector<QString> args = {"from=data/demprep/ulcn2005_lpo_downsampled.cub", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   demprep(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to prep DEM: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  EXPECT_EQ(cube.sampleCount(), 439);
  EXPECT_EQ(cube.lineCount(), 221);
  EXPECT_EQ(cube.bandCount(), 1);

  // Pixels Group
  PvlGroup &pixels = isisLabel->findGroup("Pixels", Pvl::Traverse);
  EXPECT_EQ(pixels["Type"][0].toStdString(), "SignedWord");
  EXPECT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  EXPECT_EQ(double(pixels["Base"]), 1737400.0);
  EXPECT_EQ(double(pixels["Multiplier"]), 1.0);

  // BandBin Group
  // Check size, first, 2 middle, and last values? Enough?
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(bandbin["Center"].size(), 1);
  EXPECT_EQ(bandbin["OriginalBand"].size(), 1);

  // Mapping Group
  PvlGroup &mapping = isisLabel->findGroup("Mapping", Pvl::Traverse);
  EXPECT_EQ(mapping["ProjectionName"][0].toStdString(), "Equirectangular");
  EXPECT_DOUBLE_EQ(double(mapping["CenterLongitude"]), 180.0);
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Moon");
  EXPECT_DOUBLE_EQ(double(mapping["EquatorialRadius"]), 1737400.0);
  EXPECT_DOUBLE_EQ(double(mapping["PolarRadius"]), 1737400.0);
  EXPECT_EQ(mapping["LatitudeType"][0].toStdString(), "Planetocentric");
  EXPECT_EQ(mapping["LongitudeDirection"][0].toStdString(), "PositiveEast");
  EXPECT_EQ(int(mapping["LongitudeDomain"]), 180);
  EXPECT_DOUBLE_EQ(double(mapping["MinimumLatitude"]), -90.0);
  EXPECT_DOUBLE_EQ(double(mapping["MaximumLatitude"]), 90.0);
  EXPECT_DOUBLE_EQ(double(mapping["MinimumLongitude"]), -180.0);
  EXPECT_DOUBLE_EQ(double(mapping["MaximumLongitude"]), 180.0);
  EXPECT_DOUBLE_EQ(double(mapping["UpperLeftCornerX"]), -10950000.0);
  EXPECT_DOUBLE_EQ(double(mapping["UpperLeftCornerY"]), 2775000.0);
  EXPECT_DOUBLE_EQ(double(mapping["PixelResolution"]), 25000.0);
  EXPECT_NEAR(double(mapping["Scale"]), 1.21293, .00001);
  EXPECT_DOUBLE_EQ(double(mapping["CenterLatitude"]), 0.0);

  Table shapeModel = cube.readTable("ShapeModelStatistics");
  // Assertion for minimum radius
  EXPECT_DOUBLE_EQ(double(shapeModel[0][0]), 1728.805);
  // Assertion for maximum radius
  EXPECT_DOUBLE_EQ(double(shapeModel[0][1]), 1745.313);


  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_NEAR(hist->Average(), 1736765.71744, .00001);
  EXPECT_DOUBLE_EQ(hist->Sum(), 166974392841);
  EXPECT_EQ(hist->ValidPixels(), 96141);
  EXPECT_NEAR(hist->StandardDeviation(), 2055.78, .01);
}

TEST_F(TempTestingFiles, DemprepRadius) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/padded.cub";
  QString fromFileName = "data/demprep/ulcn2005_lpo_downsampled.no_base.cub";

  QVector<QString> args = {"from=" + fromFileName, "to=" + cubeFileName, "SPHERICALDATUMRADIUS=1737400" };

  UserInterface options(APP_XML, args);
  try {
   demprep(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to prep DEM: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  EXPECT_EQ(cube.sampleCount(), 439);
  EXPECT_EQ(cube.lineCount(), 221);
  EXPECT_EQ(cube.bandCount(), 1);

  // Pixels Group
  PvlGroup &pixels = isisLabel->findGroup("Pixels", Pvl::Traverse);
  EXPECT_EQ(pixels["Type"][0].toStdString(), "SignedWord");
  EXPECT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  EXPECT_EQ(double(pixels["Base"]), 1737400.0);
  EXPECT_EQ(double(pixels["Multiplier"]), 1.0);

  // BandBin Group
  // Check size, first, 2 middle, and last values? Enough?
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(bandbin["Center"].size(), 1);
  EXPECT_EQ(bandbin["OriginalBand"].size(), 1);

  // Mapping Group
  PvlGroup &mapping = isisLabel->findGroup("Mapping", Pvl::Traverse);
  EXPECT_EQ(mapping["ProjectionName"][0].toStdString(), "Equirectangular");
  EXPECT_DOUBLE_EQ(double(mapping["CenterLongitude"]), 180.0);
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Moon");
  EXPECT_DOUBLE_EQ(double(mapping["EquatorialRadius"]), 1737400.0);
  EXPECT_DOUBLE_EQ(double(mapping["PolarRadius"]), 1737400.0);
  EXPECT_EQ(mapping["LatitudeType"][0].toStdString(), "Planetocentric");
  EXPECT_EQ(mapping["LongitudeDirection"][0].toStdString(), "PositiveEast");
  EXPECT_EQ(int(mapping["LongitudeDomain"]), 180);
  EXPECT_DOUBLE_EQ(double(mapping["MinimumLatitude"]), -90.0);
  EXPECT_DOUBLE_EQ(double(mapping["MaximumLatitude"]), 90.0);
  EXPECT_DOUBLE_EQ(double(mapping["MinimumLongitude"]), -180.0);
  EXPECT_DOUBLE_EQ(double(mapping["MaximumLongitude"]), 180.0);
  EXPECT_DOUBLE_EQ(double(mapping["UpperLeftCornerX"]), -10950000.0);
  EXPECT_DOUBLE_EQ(double(mapping["UpperLeftCornerY"]), 2775000.0);
  EXPECT_DOUBLE_EQ(double(mapping["PixelResolution"]), 25000.0);
  EXPECT_NEAR(double(mapping["Scale"]), 1.21293, .00001);
  EXPECT_DOUBLE_EQ(double(mapping["CenterLatitude"]), 0.0);

  Table shapeModel = cube.readTable("ShapeModelStatistics");
  // Assertion for minimum radius
  EXPECT_DOUBLE_EQ(double(shapeModel[0][0]), 1728.805);
  // Assertion for maximum radius
  EXPECT_DOUBLE_EQ(double(shapeModel[0][1]), 1745.313);


  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_NEAR(hist->Average(), 1736765.71744, .00001);
  EXPECT_DOUBLE_EQ(hist->Sum(), 166974392841);
  EXPECT_EQ(hist->ValidPixels(), 96141);
  EXPECT_NEAR(hist->StandardDeviation(), 2055.78, .01);
}



TEST(Demprep, DemprepInside){

  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/padded.cub";
  QVector<QString> args = {"from=data/demprep/ulcn2005_lpo_inside.cub", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   demprep(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to prep DEM: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();
  EXPECT_EQ(cube.sampleCount(), 250);
  EXPECT_EQ(cube.lineCount(), 250);
  EXPECT_EQ(cube.bandCount(), 1);

  // Mapping Group
  PvlGroup &mapping = isisLabel->findGroup("Mapping", Pvl::Traverse);
  EXPECT_EQ(mapping["ProjectionName"][0].toStdString(), "SimpleCylindrical");
  EXPECT_NEAR(double(mapping["UpperLeftCornerX"]), -5801235.97802, .00001);
  EXPECT_NEAR(double(mapping["UpperLeftCornerY"]), 77703.58546, .00001);
  EXPECT_NEAR(double(mapping["PixelResolution"]), 1895.20940, .00001);
  EXPECT_DOUBLE_EQ(double(mapping["Scale"]), 16.0);
}



TEST(Demprep, DemprepSpecialPixels){
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/padded.cub";
  QVector<QString> args = {"from=data/demprep/vest64_dtm_specialpixels_downsampled.cub", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   demprep(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to prep DEM: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();
  EXPECT_EQ(cube.sampleCount(), 366);
  EXPECT_EQ(cube.lineCount(), 184);
  EXPECT_EQ(cube.bandCount(), 1);

  // Mapping Group
  PvlGroup &mapping = isisLabel->findGroup("Mapping", Pvl::Traverse);
  EXPECT_EQ(mapping["ProjectionName"][0].toStdString(), "Equirectangular");
  EXPECT_DOUBLE_EQ(double(mapping["UpperLeftCornerX"]), -915000.0);
  EXPECT_DOUBLE_EQ(double(mapping["UpperLeftCornerY"]), 460000.0);
  EXPECT_DOUBLE_EQ(double(mapping["PixelResolution"]), 5000);
  EXPECT_NEAR(double(mapping["Scale"]), 1.00880, .00001);


  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_NEAR(double(hist->Average()), 254239.25400, .00001);
  EXPECT_DOUBLE_EQ(hist->Sum(), 3869267206.65625);
  EXPECT_EQ(hist->ValidPixels(), 15219);
  EXPECT_EQ(hist->NullPixels(), 51665);
  EXPECT_EQ(hist->LisPixels(), 460);
  EXPECT_EQ(hist->LrsPixels(), 0);
  EXPECT_EQ(hist->HisPixels(), 0);
  EXPECT_EQ(hist->HrsPixels(), 0);
  EXPECT_NEAR(hist->StandardDeviation(), 22217.85549, .00001);
}


TEST(Demprep, DemprepSouthPole){
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/padded.cub";
  QVector<QString> args = {"from=data/demprep/ulcn2005_lpo_npole.cub", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   demprep(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to prep DEM: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  EXPECT_EQ(cube.sampleCount(), 250);
  EXPECT_EQ(cube.lineCount(), 251);
  EXPECT_EQ(cube.bandCount(), 1);

  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_NEAR(double(hist->Average()), 1737016.52267, .00001);
  EXPECT_EQ(hist->Sum(), 108997786798);
  EXPECT_EQ(hist->ValidPixels(), 62750);
  EXPECT_EQ(hist->NullPixels(), 0);
  EXPECT_NEAR(hist->StandardDeviation(), 449.297, .001);
}



TEST(Demprep, DemprepNorthPole){
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/padded.cub";
  QVector<QString> args = {"from=data/demprep/ulcn2005_lpo_npole.cub", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   demprep(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to prep DEM: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  EXPECT_EQ(cube.sampleCount(), 250);
  EXPECT_EQ(cube.lineCount(), 251);
  EXPECT_EQ(cube.bandCount(), 1);

  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_NEAR(double(hist->Average()), 1737016.52267, .00001);
  EXPECT_DOUBLE_EQ(hist->Sum(), 108997786798);
  EXPECT_EQ(hist->ValidPixels(), 62750);
  EXPECT_EQ(hist->NullPixels(), 0);
  EXPECT_NEAR(hist->StandardDeviation(), 449.297, .001);
}
