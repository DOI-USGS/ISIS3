#include "StatsFunc.h"

#include "Cube.h"
#include "FileName.h"
#include "Pvl.h"
#include "SpecialPixel.h"

#include <gtest/gtest.h>

using namespace Isis;

class StatsFunc_SimpleCubeTest : public ::testing::Test {
  protected:
    Cube *testCube;

    void SetUp() override {
      // This seems extraneous, but if an exception is thrown in the Cube
      // constructor, then testCube may be non-NULL and un-assigned.
      // This would cause TearDown to seg fault.
      testCube = NULL;
      testCube = new Cube(FileName("$ISIS3DATA/base/testData/isisTruth.cub"));
    }

    void TearDown() override {
      if (testCube) {
        delete testCube;
        testCube = NULL;
      }
    }
};


TEST_F(StatsFunc_SimpleCubeTest, DefaultStats) {
  Pvl statsPvl = stats(testCube, Isis::ValidMinimum, Isis::ValidMaximum);
  ASSERT_EQ(statsPvl.groups(), 2);

  PvlGroup band1Stats = statsPvl.group(0);
  EXPECT_TRUE(band1Stats.findKeyword("Band").isEquivalent("1"));
  EXPECT_TRUE(band1Stats.findKeyword("Average").isEquivalent("0.0"));
  EXPECT_TRUE(band1Stats.findKeyword("StandardDeviation").isEquivalent("3.10108754392649e+19"));
  EXPECT_TRUE(band1Stats.findKeyword("Variance").isEquivalent("9.61674395509603e+38"));
  EXPECT_TRUE(band1Stats.findKeyword("Median").isEquivalent("1.52590222025006e+15"));
  EXPECT_TRUE(band1Stats.findKeyword("Mode").isEquivalent("1.52590222025006e+15"));
  EXPECT_TRUE(band1Stats.findKeyword("Skew").isEquivalent("-1.47616170001897e-04"));
  EXPECT_TRUE(band1Stats.findKeyword("Minimum").isEquivalent("-1.00000002004088e+20"));
  EXPECT_TRUE(band1Stats.findKeyword("Maximum").isEquivalent("1.00000002004088e+20"));
  EXPECT_TRUE(band1Stats.findKeyword("Sum").isEquivalent("0.0"));
  EXPECT_TRUE(band1Stats.findKeyword("TotalPixels").isEquivalent("15876"));
  EXPECT_TRUE(band1Stats.findKeyword("ValidPixels").isEquivalent("7056"));
  EXPECT_TRUE(band1Stats.findKeyword("OverValidMaximumPixels").isEquivalent("0"));
  EXPECT_TRUE(band1Stats.findKeyword("UnderValidMinimumPixels").isEquivalent("0"));
  EXPECT_TRUE(band1Stats.findKeyword("NullPixels").isEquivalent("1764"));
  EXPECT_TRUE(band1Stats.findKeyword("LisPixels").isEquivalent("1764"));
  EXPECT_TRUE(band1Stats.findKeyword("LrsPixels").isEquivalent("1764"));
  EXPECT_TRUE(band1Stats.findKeyword("HisPixels").isEquivalent("1764"));
  EXPECT_TRUE(band1Stats.findKeyword("HrsPixels").isEquivalent("1764"));

  PvlGroup band2Stats = statsPvl.group(1);
  EXPECT_TRUE(band2Stats.findKeyword("Band").isEquivalent("2"));
  EXPECT_TRUE(band2Stats.findKeyword("Average").isEquivalent("0.0"));
  EXPECT_TRUE(band2Stats.findKeyword("StandardDeviation").isEquivalent("3.10108754392649e+19"));
  EXPECT_TRUE(band2Stats.findKeyword("Variance").isEquivalent("9.61674395509603e+38"));
  EXPECT_TRUE(band2Stats.findKeyword("Median").isEquivalent("1.52590222025006e+15"));
  EXPECT_TRUE(band2Stats.findKeyword("Mode").isEquivalent("1.52590222025006e+15"));
  EXPECT_TRUE(band2Stats.findKeyword("Skew").isEquivalent("-1.47616170001897e-04"));
  EXPECT_TRUE(band2Stats.findKeyword("Minimum").isEquivalent("-1.00000002004088e+20"));
  EXPECT_TRUE(band2Stats.findKeyword("Maximum").isEquivalent("1.00000002004088e+20"));
  EXPECT_TRUE(band2Stats.findKeyword("Sum").isEquivalent("0.0"));
  EXPECT_TRUE(band2Stats.findKeyword("TotalPixels").isEquivalent("15876"));
  EXPECT_TRUE(band2Stats.findKeyword("ValidPixels").isEquivalent("7056"));
  EXPECT_TRUE(band2Stats.findKeyword("OverValidMaximumPixels").isEquivalent("0"));
  EXPECT_TRUE(band2Stats.findKeyword("UnderValidMinimumPixels").isEquivalent("0"));
  EXPECT_TRUE(band2Stats.findKeyword("NullPixels").isEquivalent("1764"));
  EXPECT_TRUE(band2Stats.findKeyword("LisPixels").isEquivalent("1764"));
  EXPECT_TRUE(band2Stats.findKeyword("LrsPixels").isEquivalent("1764"));
  EXPECT_TRUE(band2Stats.findKeyword("HisPixels").isEquivalent("1764"));
  EXPECT_TRUE(band2Stats.findKeyword("HrsPixels").isEquivalent("1764"));
}
