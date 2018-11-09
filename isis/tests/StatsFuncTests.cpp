#include "StatsFunc.h"

#include <iostream>

#include <gtest/gtest.h>

#include "Cube.h"
#include "FileName.h"
#include "Pvl.h"
#include "SpecialPixel.h"

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

class StatsFunc_FlatFileTest : public ::testing::Test {
  protected:
    Pvl testPvl;

    void SetUp() override {
      PvlGroup firstGroup("FirstGroup");
      firstGroup += PvlKeyword("NumberKey", "0.0");
      firstGroup += PvlKeyword("StringKey", "Hello");
      testPvl += firstGroup;

      PvlGroup secondGroup("SecondGroup");
      PvlKeyword dupKey("DuplicateKey", "stats here");
      secondGroup += dupKey;
      secondGroup += dupKey;
      testPvl += secondGroup;
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
  EXPECT_TRUE(band2Stats.findKeyword("StandardDeviation").isEquivalent("3.10108754392648e+19"));
  EXPECT_TRUE(band2Stats.findKeyword("Variance").isEquivalent("9.61674395509598e+38"));
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

TEST_F(StatsFunc_SimpleCubeTest, ValidMinimum) {
  Pvl statsPvl = stats(testCube, 0.0, Isis::ValidMaximum);

  ASSERT_TRUE(statsPvl.groups() > 0);

  PvlGroup band1Stats = statsPvl.group(0);
  EXPECT_TRUE(band1Stats.findKeyword("Average").isEquivalent("8.97436438192763e+18"));
  EXPECT_TRUE(band1Stats.findKeyword("StandardDeviation").isEquivalent("2.36768266129596e+19"));
  EXPECT_TRUE(band1Stats.findKeyword("Variance").isEquivalent("5.6059211846015e+38"));
  EXPECT_TRUE(band1Stats.findKeyword("Median").isEquivalent("0.0"));
  EXPECT_TRUE(band1Stats.findKeyword("Mode").isEquivalent("0.0"));
  EXPECT_TRUE(band1Stats.findKeyword("Skew").isEquivalent("1.1371073322405"));
  EXPECT_TRUE(band1Stats.findKeyword("Minimum").isEquivalent("0.0"));
  EXPECT_TRUE(band1Stats.findKeyword("Sum").isEquivalent("4.7492336309161e+22"));
  EXPECT_TRUE(band1Stats.findKeyword("ValidPixels").isEquivalent("5292"));
  EXPECT_TRUE(band1Stats.findKeyword("UnderValidMinimumPixels").isEquivalent("1764"));
}

TEST_F(StatsFunc_SimpleCubeTest, ValidMaximum) {
  Pvl statsPvl = stats(testCube, Isis::ValidMinimum, 0.0);

  ASSERT_TRUE(statsPvl.groups() > 0);

  PvlGroup band1Stats = statsPvl.group(0);
  EXPECT_TRUE(band1Stats.findKeyword("Average").isEquivalent("-1.29973553117573e+19"));
  EXPECT_TRUE(band1Stats.findKeyword("StandardDeviation").isEquivalent("2.75618988835977e+19"));
  EXPECT_TRUE(band1Stats.findKeyword("Variance").isEquivalent("7.59658270069666e+38"));
  EXPECT_TRUE(band1Stats.findKeyword("Median").isEquivalent("-6681.625"));
  EXPECT_TRUE(band1Stats.findKeyword("Mode").isEquivalent("-6681.625"));
  EXPECT_TRUE(band1Stats.findKeyword("Skew").isEquivalent("-1.4147089828588"));
  EXPECT_TRUE(band1Stats.findKeyword("Maximum").isEquivalent("0.0"));
  EXPECT_TRUE(band1Stats.findKeyword("Sum").isEquivalent("-4.7492336309161e+22"));
  EXPECT_TRUE(band1Stats.findKeyword("ValidPixels").isEquivalent("3654"));
  EXPECT_TRUE(band1Stats.findKeyword("OverValidMaximumPixels").isEquivalent("3402"));
}

TEST_F(StatsFunc_FlatFileTest, FlatFile) {
  std::ostringstream *testStream = new std::ostringstream();
  writeStatsStream(testPvl, false, testStream);
  EXPECT_EQ(testStream->str(), "0.0,Hello\nstats here,stats here\n");

  delete testStream;
  testStream = NULL;
}

TEST_F(StatsFunc_FlatFileTest, FlatFileHeader) {
  std::ostringstream *testStream = new std::ostringstream();
  writeStatsStream(testPvl, true, testStream);
  EXPECT_EQ(testStream->str(), "NumberKey,StringKey\n0.0,Hello\nstats here,stats here\n");

  delete testStream;
  testStream = NULL;
}
