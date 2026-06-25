#include <QString>
#include <QTemporaryDir>

#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "TempFixtures.h"
#include "TestUtilities.h"

#include "maplab.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Isis;
using ::testing::HasSubstr;

static QString APP_XML = FileName("$ISISROOT/bin/xml/maplab.xml").expanded();

/**
 * Fixture for maplab tests. maplab only reads and rewrites the Mapping group in
 * a cube's label, so the input cube's DN content is irrelevant; a tiny generated
 * cube and an in-memory map file fully exercise the feature.
 */
class MaplabTest : public TempTestingFiles {
  protected:
    QString cubeFileName;
    QString mapFileName;

    void SetUp() override {
      TempTestingFiles::SetUp();
      cubeFileName = tempDir.path() + "/maplab.cub";
      mapFileName = tempDir.path() + "/mapFile.map";
    }

    // Create a small map-projected cube with no Mapping group in its label.
    void createCube(int samples = 100, int lines = 100) {
      Cube cube;
      cube.setDimensions(samples, lines, 1);
      cube.create(cubeFileName);
      cube.close();
    }

    // Build a Sinusoidal/Mars map PVL group with the requested scale keywords.
    PvlGroup baseMapping() {
      PvlGroup mapGrp("Mapping");
      mapGrp += PvlKeyword("ProjectionName", "Sinusoidal");
      mapGrp += PvlKeyword("CenterLongitude", "227.95679821639");
      mapGrp += PvlKeyword("TargetName", "Mars");
      mapGrp += PvlKeyword("EquatorialRadius", "3396190.0", "meters");
      mapGrp += PvlKeyword("PolarRadius", "3376200.0", "meters");
      mapGrp += PvlKeyword("LatitudeType", "Planetocentric");
      mapGrp += PvlKeyword("LongitudeDirection", "PositiveEast");
      mapGrp += PvlKeyword("LongitudeDomain", "360");
      mapGrp += PvlKeyword("MinimumLatitude", "10.76690271209");
      mapGrp += PvlKeyword("MaximumLatitude", "34.444196777763");
      mapGrp += PvlKeyword("MinimumLongitude", "219.72404560653");
      mapGrp += PvlKeyword("MaximumLongitude", "236.18955082624");
      mapGrp += PvlKeyword("TrueScaleLatitude", "0.0");
      return mapGrp;
    }

    void writeMap(const PvlGroup &mapGrp) {
      Pvl map;
      map.addGroup(mapGrp);
      map.write(mapFileName);
    }

    // Return the Mapping group written into the cube label by maplab.
    PvlGroup outputMapping() {
      Cube cube(cubeFileName, "r");
      return cube.label()->findObject("IsisCube").findGroup("Mapping");
    }
};


/**
 * Geo-reference using the XY coordinate option. Verifies that
 * UpperLeftCornerX/Y are computed from the supplied (X, Y), (SAMPLE, LINE) and
 * PixelResolution, and that the Mapping group is added to the cube label.
 * Mirrors legacy case01 (maplabTruth11).
 */
TEST_F(MaplabTest, XYWithPixelResolution) {
  createCube();
  PvlGroup mapGrp = baseMapping();
  mapGrp += PvlKeyword("PixelResolution", "1000.0", "meters/pixel");
  mapGrp += PvlKeyword("Scale", "59.274697523306", "pixels/degree");
  writeMap(mapGrp);

  QVector<QString> args = {"from=" + cubeFileName, "map=" + mapFileName,
                           "coordinates=XY", "x=-395500.0", "y=2019500.0",
                           "line=23.0", "sample=85.0"};
  UserInterface ui(APP_XML, args);
  try {
    maplab(ui);
  }
  catch (IException &e) {
    FAIL() << "Unable to run maplab: " << e.what() << std::endl;
  }

  PvlGroup mapping = outputMapping();

  // x = -395500 - 1000 * (85 - 0.5) = -480000
  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerX"), -480000.0, 1e-6);
  // y =  2019500 + 1000 * (23 - 0.5) = 2042000
  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerY"), 2042000.0, 1e-6);
  EXPECT_NEAR((double)mapping.findKeyword("PixelResolution"), 1000.0, 1e-6);
  EXPECT_NEAR((double)mapping.findKeyword("Scale"), 59.274697523306, 1e-6);

  // Projection metadata is preserved from the map file.
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,
                      (QString)mapping.findKeyword("ProjectionName"), "Sinusoidal");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,
                      (QString)mapping.findKeyword("TargetName"), "Mars");
  EXPECT_NEAR((double)mapping.findKeyword("EquatorialRadius"), 3396190.0, 1e-6);
}


/**
 * Geo-reference using the LATLON coordinate option. The (LAT, LON) is projected
 * to an (X, Y) which is then offset by (SAMPLE, LINE). The chosen lat/lon
 * corresponds to the same corner as the XY case, so the result matches legacy
 * case01 (maplabTruth12).
 */
TEST_F(MaplabTest, LatLon) {
  createCube();
  PvlGroup mapGrp = baseMapping();
  mapGrp += PvlKeyword("PixelResolution", "1000.0", "meters/pixel");
  mapGrp += PvlKeyword("Scale", "59.274697523306", "pixels/degree");
  writeMap(mapGrp);

  QVector<QString> args = {"from=" + cubeFileName, "map=" + mapFileName,
                           "coordinates=latlon",
                           "lat=34.070186510964", "lon=219.90185944016",
                           "line=23.0", "sample=85.0"};
  UserInterface ui(APP_XML, args);
  try {
    maplab(ui);
  }
  catch (IException &e) {
    FAIL() << "Unable to run maplab: " << e.what() << std::endl;
  }

  PvlGroup mapping = outputMapping();

  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerX"), -480000.0, 1e-3);
  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerY"), 2042000.0, 1e-3);
  EXPECT_NEAR((double)mapping.findKeyword("PixelResolution"), 1000.0, 1e-6);
  EXPECT_NEAR((double)mapping.findKeyword("Scale"), 59.274697523306, 1e-6);
}


/**
 * When the map file supplies Scale but no PixelResolution, maplab derives the
 * resolution and adds the PixelResolution keyword to the output. Mirrors legacy
 * case03 (maplabTruth31), which uses a Scale-only map file.
 */
TEST_F(MaplabTest, ScaleOnlyDerivesPixelResolution) {
  createCube();
  PvlGroup mapGrp = baseMapping();
  mapGrp += PvlKeyword("Scale", "59.274697523306", "pixels/degree");
  writeMap(mapGrp);

  QVector<QString> args = {"from=" + cubeFileName, "map=" + mapFileName,
                           "coordinates=XY", "x=-3000.0", "y=201.0",
                           "line=23.0", "sample=85.0"};
  UserInterface ui(APP_XML, args);
  try {
    maplab(ui);
  }
  catch (IException &e) {
    FAIL() << "Unable to run maplab: " << e.what() << std::endl;
  }

  PvlGroup mapping = outputMapping();

  // Derived resolution: 2*pi*localRadius / (360 * scale) = 1000 meters.
  ASSERT_TRUE(mapping.hasKeyword("PixelResolution"));
  EXPECT_NEAR((double)mapping.findKeyword("PixelResolution"), 1000.0, 1e-6);
  EXPECT_NEAR((double)mapping.findKeyword("Scale"), 59.274697523306, 1e-6);

  // x = -3000 - 1000 * (85 - 0.5) = -87500
  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerX"), -87500.0, 1e-3);
  // y =  201 + 1000 * (23 - 0.5) = 22701
  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerY"), 22701.0, 1e-3);
}


/**
 * An existing Mapping group in the input cube is replaced, not duplicated.
 */
TEST_F(MaplabTest, ReplacesExistingMappingGroup) {
  // Create a cube that already has a (stale) Mapping group.
  {
    Cube cube;
    cube.setDimensions(100, 100, 1);
    cube.create(cubeFileName);
    PvlGroup stale("Mapping");
    stale += PvlKeyword("ProjectionName", "Equirectangular");
    stale += PvlKeyword("UpperLeftCornerX", "999999.0", "meters");
    cube.label()->findObject("IsisCube").addGroup(stale);
    cube.close();
  }

  PvlGroup mapGrp = baseMapping();
  mapGrp += PvlKeyword("PixelResolution", "1000.0", "meters/pixel");
  writeMap(mapGrp);

  QVector<QString> args = {"from=" + cubeFileName, "map=" + mapFileName,
                           "coordinates=XY", "x=-395500.0", "y=2019500.0",
                           "line=23.0", "sample=85.0"};
  UserInterface ui(APP_XML, args);
  try {
    maplab(ui);
  }
  catch (IException &e) {
    FAIL() << "Unable to run maplab: " << e.what() << std::endl;
  }

  Cube cube(cubeFileName, "r");
  PvlObject &isisCube = cube.label()->findObject("IsisCube");

  // Exactly one Mapping group, now Sinusoidal with the recomputed corner.
  int mappingCount = 0;
  for (auto grp = isisCube.beginGroup(); grp != isisCube.endGroup(); grp++) {
    if (grp->name() == "Mapping") {
      mappingCount++;
    }
  }
  EXPECT_EQ(mappingCount, 1);

  PvlGroup &mapping = isisCube.findGroup("Mapping");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,
                      (QString)mapping.findKeyword("ProjectionName"), "Sinusoidal");
  EXPECT_NEAR((double)mapping.findKeyword("UpperLeftCornerX"), -480000.0, 1e-6);
}


/**
 * A map file missing the TargetName keyword is rejected.
 */
TEST_F(MaplabTest, ErrorMissingTargetName) {
  createCube();
  PvlGroup mapGrp("Mapping");
  mapGrp += PvlKeyword("ProjectionName", "Sinusoidal");
  mapGrp += PvlKeyword("EquatorialRadius", "3396190.0", "meters");
  mapGrp += PvlKeyword("PolarRadius", "3376200.0", "meters");
  mapGrp += PvlKeyword("LongitudeDomain", "360");
  mapGrp += PvlKeyword("PixelResolution", "1000.0", "meters/pixel");
  writeMap(mapGrp);

  QVector<QString> args = {"from=" + cubeFileName, "map=" + mapFileName,
                           "coordinates=XY", "x=0.0", "y=0.0",
                           "line=1.0", "sample=1.0"};
  UserInterface ui(APP_XML, args);
  try {
    maplab(ui);
    FAIL() << "Expected an exception for missing TargetName" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.toString().toStdString(), HasSubstr("does not have the TargetName"));
  }
}


/**
 * A map file with neither PixelResolution nor Scale is rejected.
 */
TEST_F(MaplabTest, ErrorMissingResolutionAndScale) {
  createCube();
  writeMap(baseMapping());

  QVector<QString> args = {"from=" + cubeFileName, "map=" + mapFileName,
                           "coordinates=XY", "x=0.0", "y=0.0",
                           "line=1.0", "sample=1.0"};
  UserInterface ui(APP_XML, args);
  try {
    maplab(ui);
    FAIL() << "Expected an exception for missing PixelResolution/Scale" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.toString().toStdString(),
                HasSubstr("does not have the PixelResolution or Scale"));
  }
}
