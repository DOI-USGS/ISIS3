#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "apollocal.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/apollocal.xml").expanded();

TEST_F(ApolloCube, FunctionalTestApollocalDefault) {
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

  Cube oCube(outCubeFileName, "r");

  std::unique_ptr<Histogram> oCubeStats(oCube.histogram());

  EXPECT_NEAR(oCubeStats->Average(), -124.188,          0.001);
  EXPECT_NEAR(oCubeStats->Sum(), -65125546242.836,   0.001);
  EXPECT_NEAR(oCubeStats->ValidPixels(), 524410000,     0.001);
  EXPECT_NEAR(oCubeStats->StandardDeviation(), 1056.736, 0.001);
}
