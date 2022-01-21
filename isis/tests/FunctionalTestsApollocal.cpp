#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include <chrono>

#include "apollocal.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/apollocal.xml").expanded();

TEST_F(ApolloCube, FunctionalTestApollocalDefault) {
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  QTemporaryDir prefix;

  QString outCubeFileName = prefix.path()+"/outTEMP.cub";
  QVector<QString> args = {"to="+outCubeFileName};

  UserInterface options(APP_XML, args);

  try {
    apollocal(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Call failed, Unable to process cube: " << e.what() << std::endl;
  }
  std::chrono::steady_clock::time_point after_cal = std::chrono::steady_clock::now();

  Cube oCube(outCubeFileName, "r");

  std::unique_ptr<Histogram> oCubeStats(oCube.histogram());

  EXPECT_NEAR(oCubeStats->Average(), -124.188,          0.001);
  EXPECT_NEAR(oCubeStats->Sum(), -65125546242.836,   0.001);
  EXPECT_NEAR(oCubeStats->ValidPixels(), 524410000,     0.001);
  EXPECT_NEAR(oCubeStats->StandardDeviation(), 1056.736, 0.001);
  std::chrono::steady_clock::time_point after_check = std::chrono::steady_clock::now();

  std::cout << "Time to setup: " << (double)std::chrono::duration_cast<std::chrono::milliseconds>(begin - start_time).count() / 1000 << " [s]" << std::endl;
  std::cout << "Time to calibrate: " << (double)std::chrono::duration_cast<std::chrono::milliseconds>(after_cal - begin).count() / 1000 << " [s]" << std::endl;
  std::cout << "Time to check: " << (double)std::chrono::duration_cast<std::chrono::milliseconds>(after_check - after_cal).count() / 1000 << " [s]" << std::endl;
  FAIL();
}
