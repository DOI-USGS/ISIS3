#include "noseam.h"

#include "NetworkFixtures.h"
#include "Pvl.h"
#include "gmock/gmock.h"

using namespace std;
using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/noseam.xml").expanded();

/**
   * NoseamDefault Test using ThreeImageNetwork Fixture
   * 
   * INPUT: 1) FileName of cube list with cube1map, cube2map, cube3map
   *           from ThreeImageNetwork Fixture
   *        2) Boxcar dimensions
   *           samples = 11
   *           lines = 11
   *        3) matchbandbin = yes (default)
   *        4) removetemp = yes (default)
   * 
   * OUTPUT: noseamDefaultOut.cub
  */
TEST_F(ThreeImageNetwork, FunctionalTestNoseamDefault) {

  // create list of input projected cube files
  FileName cubeListFileName(tempDir.path() + "/cubes.lis");

  ofstream of;
  of.open((cubeListFileName.original()).toStdString());
  of << cube1map->fileName() << "\n";
  of << cube2map->fileName() << "\n";
  of << cube3map->fileName() << "\n";
  of.close();

  // run noseam
  QVector<QString> args = {"to=" + tempDir.path() + "/noseamDefaultOut.cub",
                           "samples=11",
                           "lines=11"
                           };

  UserInterface ui(APP_XML, args);

  try {
    noseam(cubeListFileName, ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }
 
  // validate output mosaic
  Cube mosaic(tempDir.path() + "/noseamDefaultOut.cub");

  PvlObject mosaicLabel = mosaic.label()->findObject("IsisCube");
  PvlGroup dimensions = mosaicLabel.findObject("Core").findGroup("Dimensions");
  PvlGroup pixels = mosaicLabel.findObject("Core").findGroup("Pixels");
  PvlGroup mapping = mosaicLabel.findGroup("Mapping");

  EXPECT_EQ(int(dimensions["Samples"]), 548);
  EXPECT_EQ(int(dimensions["Lines"]), 487);
  EXPECT_EQ(int(dimensions["Bands"]), 1);

  EXPECT_EQ(pixels["Type"][0], "Real");
  EXPECT_EQ(pixels["ByteOrder"][0], "Lsb");
  EXPECT_EQ(double(pixels["Base"]), 0.0);
  EXPECT_EQ(double(pixels["Multiplier"]), 1.0);

  EXPECT_EQ(double(mapping["MinimumLatitude"]), 0.47920860194551);
  EXPECT_EQ(double(mapping["MaximumLatitude"]), 3.3932951263901);
  EXPECT_EQ(double(mapping["MinimumLongitude"]), -0.94830771139743);
  EXPECT_EQ(double(mapping["MaximumLongitude"]), 1.4318179715731);
}


/**
   * NoseamEvenBoxFilterSamples Test using ThreeImageNetwork Fixture
   * 
   * INPUT: 1) FileName of cube list with cube1map, cube2map, cube3map
   *           from ThreeImageNetwork Fixture
   *        2) Boxcar dimensions
   *           samples = 12
   *           lines = 11
   *        3) matchbandbin = yes (default)
   *        4) removetemp = yes (default)
   * 
   * THROWS: **USER ERROR** Value for [SAMPLES] must be odd.
  */
TEST_F(ThreeImageNetwork, FunctionalTestNoseamEvenBoxFilterSamples) {

  // create list of input cube files
  ofstream of;
  of.open(tempDir.path().toStdString() + "/cubes.lis");
  of << cube1map->fileName() << "\n";
  of << cube2map->fileName() << "\n";
  of << cube3map->fileName() << "\n";
  of.close();

  // run noseam
  QVector<QString> args = {"fromlist=" + tempDir.path() + "/cubes.lis",
                           "to=" + tempDir.path() + "/result.cub",
                           "samples=12",
                           "lines=11"
                           };

  UserInterface ui(APP_XML, args);

  try {
    noseam(ui);
    FAIL() << "Expected Exception for boxcar even sample input";
  }
  catch (IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("[SAMPLES] must be odd"))
      << e.toString().toStdString();
  }
}


/**
   * NoseamEvenBoxFilterLines Test using ThreeImageNetwork Fixture
   * 
   * INPUT: 1) FileName of cube list with cube1map, cube2map, cube3map
   *           from ThreeImageNetwork Fixture
   *        2) Boxcar dimensions
   *           samples = 11
   *           lines = 12
   *        3) matchbandbin = yes (default)
   *        4) removetemp = yes (default)
   * 
   * THROWS: **USER ERROR** Value for [LINES] must be odd.
  */
TEST_F(ThreeImageNetwork, FunctionalTestNoseamEvenBoxFilterLines) {

  // create list of input cube files
  ofstream of;
  of.open(tempDir.path().toStdString() + "/cubes.lis");
  of << cube1map->fileName() << "\n";
  of << cube2map->fileName() << "\n";
  of << cube3map->fileName() << "\n";
  of.close();

  // run noseam
  QVector<QString> args = {"fromlist=" + tempDir.path() + "/cubes.lis",
                           "to=" + tempDir.path() + "/result.cub",
                           "samples=11",
                           "lines=12"
                           };

  UserInterface ui(APP_XML, args);

  try {
    noseam(ui);
    FAIL() << "Expected Exception for boxcar even line input";
  }
  catch (IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("[LINES] must be odd"))
      << e.toString().toStdString();
  }
}


/**
   * Noseam NoBoxFilterSamples Test using ThreeImageNetwork Fixture
   * 
   * INPUT: 1) FileName of cube list with cube1map, cube2map, cube3map
   *           from ThreeImageNetwork Fixture
   *        2) Boxcar dimensions
   *           lines = 11
   *        3) matchbandbin = yes (default)
   *        4) removetemp = yes (default)
   * 
   * THROWS: **USER ERROR** Parameter [SAMPLES] has no value.
  */
TEST_F(ThreeImageNetwork, FunctionalTestNoseamNoBoxFilterSamples) {

  // create list of input cube files
  ofstream of;
  of.open(tempDir.path().toStdString() + "/cubes.lis");
  of << cube1map->fileName() << "\n";
  of << cube2map->fileName() << "\n";
  of << cube3map->fileName() << "\n";
  of.close();

  // run noseam
  QVector<QString> args = {"fromlist=" + tempDir.path() + "/cubes.lis",
                           "to=" + tempDir.path() + "/result.cub",
                           "lines=11"
                           };

  UserInterface ui(APP_XML, args);

  try {
    noseam(ui);
    FAIL() << "Expected Exception for no input for boxcar samples";
  }
  catch (IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("[SAMPLES] has no value"))
      << e.toString().toStdString();
  }
}


/**
   * Noseam NoBoxFilterLines Test using ThreeImageNetwork Fixture
   * 
   * INPUT: 1) FileName of cube list with cube1map, cube2map, cube3map
   *           from ThreeImageNetwork Fixture
   *        2) Boxcar dimensions
   *           samples = 11
   *        3) matchbandbin = yes (default)
   *        4) removetemp = yes (default)
   * 
   * THROWS: **USER ERROR** Parameter [LINES] has no value.
  */
TEST_F(ThreeImageNetwork, FunctionalTestNoseamNoBoxFilterLines) {

  // create list of input cube files
  ofstream of;
  of.open(tempDir.path().toStdString() + "/cubes.lis");
  of << cube1map->fileName() << "\n";
  of << cube2map->fileName() << "\n";
  of << cube3map->fileName() << "\n";
  of.close();

  // run noseam
  QVector<QString> args = {"fromlist=" + tempDir.path() + "/cubes.lis",
                           "to=" + tempDir.path() + "/result.cub",
                           "samples=11"
                           };

  UserInterface ui(APP_XML, args);

  try {
    noseam(ui);
    FAIL() << "Expected Exception for no input for boxcar lines";
  }
  catch (IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("[LINES] has no value"))
      << e.toString().toStdString();
  }
}
