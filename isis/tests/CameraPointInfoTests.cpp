/* SPDX-License-Identifier: CC0-1.0 */
#include <gtest/gtest.h>

#include "CameraPointInfo.h"
#include "FileName.h"
#include "Preference.h"
#include "PvlGroup.h"

#include <cmath>
#include <memory>

using namespace Isis;

namespace {

// Helper: return expanded cube path as QString (required by SetCube)
QString UnitTestCube() {
  return FileName("$ISISTESTDATA/isis/src/base/unitTestData/CameraPointInfo/unitTest1.cub")
      .expanded();
}

// Assert keyword exists and has expected element count
void ExpectKeywordCount(const PvlGroup &g, const QString &key, int n) {
  ASSERT_TRUE(g.hasKeyword(key)) << "Missing keyword: " << key.toStdString();
  EXPECT_EQ(g.findKeyword(key).size(), n) << "Keyword size mismatch: " << key.toStdString();
}

// Assert keyword value is finite
void ExpectFiniteKeyword(const PvlGroup &g, const QString &key, int idx = 0) {
  ASSERT_TRUE(g.hasKeyword(key)) << "Missing keyword: " << key.toStdString();
  const double v = toDouble(g.findKeyword(key)[idx]);
  EXPECT_TRUE(std::isfinite(v)) << "Non-finite value for " << key.toStdString();
}

// Test fixture
class CameraPointInfoTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    Preference::Preferences(true);
  }
};

}  // namespace

TEST_F(CameraPointInfoTest, SetImage) {
  CameraPointInfo cpi;
  cpi.SetCube(UnitTestCube());

  std::unique_ptr<PvlGroup> grp(cpi.SetImage(1, 1));
  ASSERT_NE(grp, nullptr);
  EXPECT_GT(grp->keywords(), 0);

  // Numeric sanity (also asserts keywords exist)
  ExpectFiniteKeyword(*grp, "Sample");
  ExpectFiniteKeyword(*grp, "Line");
  ExpectFiniteKeyword(*grp, "PixelValue");
  ExpectFiniteKeyword(*grp, "PlanetocentricLatitude");
  ExpectFiniteKeyword(*grp, "PositiveEast360Longitude");
  ExpectFiniteKeyword(*grp, "LocalRadius");
  ExpectFiniteKeyword(*grp, "EphemerisTime");

  // Direction vectors should be 3-vectors
  ExpectKeywordCount(*grp, "LookDirectionBodyFixed", 3);
  ExpectKeywordCount(*grp, "LookDirectionJ2000", 3);
  ExpectKeywordCount(*grp, "LookDirectionCamera", 3);

  // Geometry bounds (broad and stable)
  const double lat = toDouble(grp->findKeyword("PlanetocentricLatitude")[0]);
  const double lon = toDouble(grp->findKeyword("PositiveEast360Longitude")[0]);

  EXPECT_GE(lat, -90.0);
  EXPECT_LE(lat, 90.0);

  // 0..360 convention is common for PositiveEast360Longitude; treat 360 as exclusive
  EXPECT_GE(lon, 0.0);
  EXPECT_LT(lon, 360.0);

  // Angles: if present, should be sane
  if (grp->hasKeyword("Incidence")) {
    const double inc = toDouble(grp->findKeyword("Incidence")[0]);
    EXPECT_GE(inc, 0.0);
    EXPECT_LE(inc, 180.0);
  }
  if (grp->hasKeyword("Emission")) {
    const double em = toDouble(grp->findKeyword("Emission")[0]);
    EXPECT_GE(em, 0.0);
    EXPECT_LE(em, 180.0);
  }
  if (grp->hasKeyword("Phase")) {
    const double ph = toDouble(grp->findKeyword("Phase")[0]);
    EXPECT_GE(ph, 0.0);
    EXPECT_LE(ph, 180.0);
  }
}

TEST_F(CameraPointInfoTest, SetGround) {
  CameraPointInfo cpi;
  cpi.SetCube(UnitTestCube());

  std::unique_ptr<PvlGroup> grp(cpi.SetGround(-84.5, 15.0));
  ASSERT_NE(grp, nullptr);
  EXPECT_GT(grp->keywords(), 0);

  // Numeric sanity (also asserts keywords exist)
  ExpectFiniteKeyword(*grp, "Sample");
  ExpectFiniteKeyword(*grp, "Line");
  ExpectFiniteKeyword(*grp, "PlanetocentricLatitude");
  ExpectFiniteKeyword(*grp, "PositiveEast360Longitude");
  ExpectFiniteKeyword(*grp, "LocalRadius");
  ExpectFiniteKeyword(*grp, "EphemerisTime");

  // Direction vectors should be 3-vectors
  ExpectKeywordCount(*grp, "LookDirectionBodyFixed", 3);
  ExpectKeywordCount(*grp, "LookDirectionJ2000", 3);
  ExpectKeywordCount(*grp, "LookDirectionCamera", 3);

  // Ground point round-trip (matches SetGround inputs)
  EXPECT_NEAR(toDouble(grp->findKeyword("PlanetocentricLatitude")[0]), -84.5, 1e-3);
  EXPECT_NEAR(toDouble(grp->findKeyword("PositiveEast360Longitude")[0]), 15.0, 1e-3);

  // Optional alternative longitude representations
  if (grp->hasKeyword("PositiveEast180Longitude")) {
    EXPECT_NEAR(toDouble(grp->findKeyword("PositiveEast180Longitude")[0]), 15.0, 1e-3);
  }
  if (grp->hasKeyword("PositiveWest360Longitude")) {
    EXPECT_NEAR(toDouble(grp->findKeyword("PositiveWest360Longitude")[0]), 345.0, 1e-3);
  }
  if (grp->hasKeyword("PositiveWest180Longitude")) {
    EXPECT_NEAR(toDouble(grp->findKeyword("PositiveWest180Longitude")[0]), -15.0, 1e-3);
  }
}