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
}
