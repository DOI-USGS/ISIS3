#include "Fixtures.h"
#include "Histogram.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "lrowacphomap.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/lrowacphomap.xml").expanded();

TEST(Lrowacphomap, FunctionalTestLrowacphomapWithBack) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());
  
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QString testCubeFileName = "data/lrowacphomap/M1181493219CE.vis.odd.reduced.cub";
  QString backplaneFileName = "data/lrowacphomap/back.reduced.cub";
  QString phoPvlFileName = "data/lrowacphomap/hapke_full_reformatted.pvl";
  QString paramMapFileName = "data/lrowacphomap/1x1_70NS_7b_wbhs_albflt_grid_geirist_tcorrect.reduced.cub";

  QVector<QString> args = {"from=" + testCubeFileName + "+1", 
                           "to=" + outCubeFileName, 
                           "backplane=" + backplaneFileName, 
                           "phoa=" + phoPvlFileName,
                           "phop=" + paramMapFileName};
  UserInterface options(APP_XML, args);

  try {
    lrowacphomap(options);
  }
  catch(IException &e) {
    FAIL() << "Call to lrowacphomap failed, unable to apply photometric correction to input cube: " << e.what() << std::endl;
  }

  double expectedAvg = 58.565850201775;
  double expectedStdDev = 19.336237864721;
  double expectedMedian = 56.231717465174;
  double expectedMin = 21.181716918945;
  double expectedMax = 160.17492675781;
  double expectedSum = 197132.65177917;

  Cube outCube(outCubeFileName);

  std::unique_ptr<Histogram> hist(outCube.histogram());

  EXPECT_NEAR(hist->Average(), expectedAvg, 0.001);
  EXPECT_NEAR(hist->StandardDeviation(), expectedStdDev, 0.001);
  EXPECT_NEAR(hist->Median(), expectedMedian, 0.001);
  EXPECT_NEAR(hist->Minimum(), expectedMin, 0.001);
  EXPECT_NEAR(hist->Maximum(), expectedMax, 0.001);
  EXPECT_NEAR(hist->Sum(), expectedSum, 0.001);
}

TEST(Lrowacphomap, FunctionalTestLrowacphomapNoBack) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());
  
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QString testCubeFileName = "data/lrowacphomap/M1181493219CE.vis.odd.reduced.cub";
  QString phoPvlFileName = "data/lrowacphomap/hapke_full_reformatted.pvl";
  QString paramMapFileName = "data/lrowacphomap/1x1_70NS_7b_wbhs_albflt_grid_geirist_tcorrect.reduced.cub";

  QVector<QString> args = {"from=" + testCubeFileName + "+1", 
                           "to=" + outCubeFileName, 
                           "phoa=" + phoPvlFileName,
                           "phop=" + paramMapFileName,
                           "usedem=true"};
  UserInterface options(APP_XML, args);

  try {
    lrowacphomap(options);
  }
  catch(IException &e) {
    FAIL() << "Call to lrowacphomap failed, unable to apply photometric correction to input cube: " << e.what() << std::endl;
  }

  double expectedAvg = 58.159470616532;
  double expectedStdDev = 18.558190342074;
  double expectedMedian = 56.508963061387;
  double expectedMin = 23.405038833618;
  double expectedMax = 155.67340087891;
  double expectedSum = 195764.77809525;

  Cube outCube(outCubeFileName);

  std::unique_ptr<Histogram> hist(outCube.histogram());

  EXPECT_NEAR(hist->Average(), expectedAvg, 0.001);
  EXPECT_NEAR(hist->StandardDeviation(), expectedStdDev, 0.001);
  EXPECT_NEAR(hist->Median(), expectedMedian, 0.001);
  EXPECT_NEAR(hist->Minimum(), expectedMin, 0.001);
  EXPECT_NEAR(hist->Maximum(), expectedMax, 0.001);
  EXPECT_NEAR(hist->Sum(), expectedSum, 0.001);
}
