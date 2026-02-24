// SPDX-License-Identifier: CC0-1.0
#include <gtest/gtest.h>

#include "AlphaCube.h"
#include "Cube.h"
#include "FileName.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include <QFile>

using namespace Isis;

namespace {

class AlphaCubeTest : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    Preference::Preferences(true);
  }

  static AlphaCube MakeC() {
    // Matches legacy unit test "c"
    return AlphaCube(4, 8, 2, 3,
                     1.5, 2.5,
                     3.5, 5.5);
  }

  static AlphaCube MakeD() {
    // Matches legacy unit test "d"
    return AlphaCube(2, 3, 2, 4,
                     1.5, 1.5,
                     2.5, 3.5);
  }
};

}  // namespace

// 1st Test Alpha in the legacy unit test
TEST_F(AlphaCubeTest, FirstAlphaMatchesLegacyValues) {
  AlphaCube c = MakeC();

  // Dimensions
  EXPECT_EQ(c.AlphaSamples(), 4);
  EXPECT_EQ(c.AlphaLines(),   8);
  EXPECT_EQ(c.BetaSamples(),  2);
  EXPECT_EQ(c.BetaLines(),    3);

  // Forward mapping: AlphaSample / AlphaLine at specific beta positions
  // Analytic: sample slope = (3.5 - 1.5) / 2 = 1.0
  //           line   slope = (5.5 - 2.5) / 3 = 1.0
  constexpr double eps = 1.0e-14;

  EXPECT_NEAR(c.AlphaSample(1.0),                 2.0, eps);
  EXPECT_NEAR(c.AlphaLine(1.0),                   3.0, eps);

  EXPECT_NEAR(c.AlphaSample(c.BetaSamples()),     3.0, eps);  // betaSample = 2
  EXPECT_NEAR(c.AlphaLine(c.BetaLines()),         5.0, eps);  // betaLine   = 3

  EXPECT_NEAR(c.AlphaSample(0.5),                 1.5, eps);
  EXPECT_NEAR(c.AlphaLine(0.5),                   2.5, eps);

  EXPECT_NEAR(c.AlphaSample(c.BetaSamples() + 0.5), 3.5, eps);  // 2.5
  EXPECT_NEAR(c.AlphaLine(c.BetaLines() + 0.5),     5.5, eps);  // 3.5
}

// 1st Test Beta in the legacy unit test
TEST_F(AlphaCubeTest, FirstBetaMatchesAnalyticValues) {
  AlphaCube c = MakeC();

  EXPECT_EQ(c.BetaSamples(), 2);
  EXPECT_EQ(c.BetaLines(),   3);

  // For c: sample slope = 1.0, line slope = 1.0
  // BetaSample(alphaSample) = alphaSample - 1.0
  // BetaLine(alphaLine)     = alphaLine - 2.0

  EXPECT_DOUBLE_EQ(c.BetaSample(1.0),          0.0);
  EXPECT_DOUBLE_EQ(c.BetaLine(1.0),           -1.0);

  EXPECT_DOUBLE_EQ(c.BetaSample(c.AlphaSamples()), 3.0);  // alphaSample = 4
  EXPECT_DOUBLE_EQ(c.BetaLine(c.AlphaLines()),     6.0);  // alphaLine   = 8
}

// 2nd Alpha Test in the legacy unit test
TEST_F(AlphaCubeTest, SecondAlphaMatchesLegacyValues) {
  AlphaCube d = MakeD();

  EXPECT_EQ(d.AlphaSamples(), 2);
  EXPECT_EQ(d.AlphaLines(),   3);
  EXPECT_EQ(d.BetaSamples(),  2);
  EXPECT_EQ(d.BetaLines(),    4);

  // For d: sample slope = (2.5 - 1.5) / 2 = 0.5
  //        line   slope = (3.5 - 1.5) / 4 = 0.5
  constexpr double eps = 1.0e-14;

  EXPECT_NEAR(d.AlphaSample(1.0),                 1.75, eps);
  EXPECT_NEAR(d.AlphaLine(1.0),                   1.75, eps);

  EXPECT_NEAR(d.AlphaSample(d.BetaSamples()),     2.25, eps);  // betaSample = 2
  EXPECT_NEAR(d.AlphaLine(d.BetaLines()),         3.25, eps);  // betaLine   = 4

  EXPECT_NEAR(d.AlphaSample(0.5),                 1.5, eps);
  EXPECT_NEAR(d.AlphaLine(0.5),                   1.5, eps);

  EXPECT_NEAR(d.AlphaSample(d.BetaSamples() + 0.5), 2.5, eps);  // 2.5
  EXPECT_NEAR(d.AlphaLine(d.BetaLines() + 0.5),     3.5, eps);  // 4.5
}

// 2nd Beta Test in the legacy unit test
TEST_F(AlphaCubeTest, SecondBetaMatchesAnalyticValues) {
  AlphaCube d = MakeD();

  EXPECT_EQ(d.BetaSamples(), 2);
  EXPECT_EQ(d.BetaLines(),   4);

  // For d: sample slope = 0.5, line slope = 0.5
  // BetaSample(alphaSample) = (alphaSample - 1.5) / 0.5 + 0.5
  // BetaLine(alphaLine)     = (alphaLine  - 1.5) / 0.5 + 0.5

  EXPECT_DOUBLE_EQ(d.BetaSample(1.0),          -0.5);
  EXPECT_DOUBLE_EQ(d.BetaLine(1.0),           -0.5);

  EXPECT_DOUBLE_EQ(d.BetaSample(d.AlphaSamples()), 1.5);  // alphaSample = 2
  EXPECT_DOUBLE_EQ(d.BetaLine(d.AlphaLines()),     3.5);  // alphaLine   = 3
}

// 3rd Test Alpha (after c.Rehash(d)) in the legacy unit test.
//
// Rehash algorithm (from AlphaCube::Rehash):
//   sl = AlphaLine(add.AlphaLine(0.5));
//   ss = AlphaSample(add.AlphaSample(0.5));
//   el = AlphaLine(add.AlphaLine(add.BetaLines() + 0.5));
//   es = AlphaSample(add.AlphaSample(add.BetaSamples() + 0.5));
//
// For c and d, this yields:
//   AlphaStartingSample = 2.5
//   AlphaStartingLine   = 3.5
//   AlphaEndingSample   = 3.5
//   AlphaEndingLine     = 5.5
//   BetaSamples         = 2
//   BetaLines           = 4
TEST_F(AlphaCubeTest, RehashProducesExpectedCompositeMapping) {
  AlphaCube c = MakeC();
  AlphaCube d = MakeD();

  c.Rehash(d);

  EXPECT_EQ(c.AlphaSamples(), 4);
  EXPECT_EQ(c.AlphaLines(),   8);
  EXPECT_EQ(c.BetaSamples(),  2);
  EXPECT_EQ(c.BetaLines(),    4);

  constexpr double eps = 1.0e-14;

  // Check a few forward mapping values used in the legacy "3rd Test Alpha"
  EXPECT_NEAR(c.AlphaSample(1.0),                 2.75, eps);
  EXPECT_NEAR(c.AlphaLine(1.0),                   3.75, eps);

  EXPECT_NEAR(c.AlphaSample(c.BetaSamples()),     3.25, eps);  // betaSample = 2
  EXPECT_NEAR(c.AlphaLine(c.BetaLines()),         5.25, eps);  // betaLine   = 4

  EXPECT_NEAR(c.AlphaSample(0.5),                 2.5, eps);
  EXPECT_NEAR(c.AlphaLine(0.5),                   3.5, eps);

  EXPECT_NEAR(c.AlphaSample(c.BetaSamples() + 0.5), 3.5, eps);  // 2.5
  EXPECT_NEAR(c.AlphaLine(c.BetaLines() + 0.5),     5.5, eps);  // 4.5
}

// Final block of the legacy unit test: c.Rehash(d) followed by UpdateGroup on
// a 4x8 cube, and inspection of the AlphaCube group.
TEST_F(AlphaCubeTest, UpdateGroupWritesExpectedAlphaCubeGroup) {
  AlphaCube c = MakeC();
  AlphaCube d = MakeD();
  c.Rehash(d);

  // Create a small throwaway cube instead of using ISISTESTDATA.
  Cube cube;
  cube.setDimensions(4, 8, 1);
  cube.create("AlphaCubeTest_UpdateGroup.cub");

  Pvl &lab = *cube.label();

  lab.clear();
  lab.addObject(PvlObject("IsisCube"));

  PvlObject &isiscube = lab.findObject("IsisCube");
  isiscube.addGroup(PvlGroup("Dimensions"));

  PvlGroup &dims = isiscube.findGroup("Dimensions");
  dims += PvlKeyword("Samples", "4");
  dims += PvlKeyword("Lines",   "8");

  c.UpdateGroup(cube);

  ASSERT_TRUE(isiscube.hasGroup("AlphaCube"));
  PvlGroup &alpha = isiscube.findGroup("AlphaCube");

  EXPECT_EQ(alpha["AlphaSamples"][0], "4");
  EXPECT_EQ(alpha["AlphaLines"][0],   "8");

  EXPECT_EQ(alpha["AlphaStartingSample"][0], "2.5");
  EXPECT_EQ(alpha["AlphaStartingLine"][0],   "3.5");
  EXPECT_EQ(alpha["AlphaEndingSample"][0],   "3.5");
  EXPECT_EQ(alpha["AlphaEndingLine"][0],     "5.5");

  EXPECT_EQ(alpha["BetaSamples"][0], "2");
  EXPECT_EQ(alpha["BetaLines"][0],   "4");
  
  cube.close();
  QFile::remove("AlphaCubeTest_UpdateGroup.cub");

}

