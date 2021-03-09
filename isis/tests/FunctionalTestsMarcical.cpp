#include <QTemporaryDir>

#include "marcical.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/marcical.xml").expanded();

TEST(Marcical, MarcicalTestDefault) {
  QTemporaryDir prefix;
  // QString cubeFileName = prefix.path() + "/marcical_out.cub";
  QString cubeFileName = "data/marci2isis/marcical_out.cub";
  QVector<QString> args = {"from=data/marci2isis/P12_005901_3391_MA_00N096W_cropped.cub", "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  marcical(options);

  Cube inCube("data/marci2isis/P12_005901_3391_MA_00N096W_cropped.cub");
  Cube outCube(cubeFileName);

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  std::unique_ptr<Histogram> inHist (inCube.histogram());


  EXPECT_EQ( inHist->Average(), outHist->Average() );
  EXPECT_EQ( inHist->Sum(), outHist->Sum() );
  EXPECT_EQ( inHist->ValidPixels(), outHist->ValidPixels() );
  EXPECT_EQ( inHist->StandardDeviation(), outHist->StandardDeviation() );
  

  /*
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);

  std::unique_ptr<Histogram> inHist (inCube.histogram());
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);
  */
}

TEST(Marcical, MarcicalTestDefaultNoIof) {
  QTemporaryDir prefix;
  // QString cubeFileName = prefix.path() + "/marcical_out.cub";
  QString cubeFileName = "data/marci2isis/marcical_out2.cub";
  QVector<QString> args = {"from=data/marci2isis/P12_005901_3391_MA_00N096W_cropped.cub", "to=" + cubeFileName, "iof=no" };
  UserInterface options(APP_XML, args);
  marcical(options);

  Cube inCube("data/marci2isis/P12_005901_3391_MA_00N096W_cropped.cub");
  Cube outCube(cubeFileName);

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  std::unique_ptr<Histogram> inHist (inCube.histogram());


  EXPECT_EQ( inHist->Average(), outHist->Average() );
  EXPECT_EQ( inHist->Sum(), outHist->Sum() );
  EXPECT_EQ( inHist->ValidPixels(), outHist->ValidPixels() );
  EXPECT_EQ( inHist->StandardDeviation(), outHist->StandardDeviation() );
  

  /*
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);

  std::unique_ptr<Histogram> inHist (inCube.histogram());
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);
  */
}

TEST(Marcical, MarcicalTestSingleDuration) {
  QTemporaryDir prefix;
  // QString cubeFileName = prefix.path() + "/marcical_out.cub";
  QString cubeFileName = "data/marci2isis/marcical_out0.cub";
  QVector<QString> args = {"from=data/marci2isis/K14_059003_3475_MA_00N112W_cropped.cub", "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  marcical(options);

  Cube inCube("data/marci2isis/K14_059003_3475_MA_00N112W_cropped.cub");
  Cube outCube(cubeFileName);

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  std::unique_ptr<Histogram> inHist (inCube.histogram());


  EXPECT_EQ( inHist->Average(), outHist->Average() );
  EXPECT_EQ( inHist->Sum(), outHist->Sum() );
  EXPECT_EQ( inHist->ValidPixels(), outHist->ValidPixels() );
  EXPECT_EQ( inHist->StandardDeviation(), outHist->StandardDeviation() );
  

  /*
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);

  std::unique_ptr<Histogram> inHist (inCube.histogram());
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);
  */
}

TEST(Marcical, MarcicalTestSingleDurationNoIof) {
  QTemporaryDir prefix;
  // QString cubeFileName = prefix.path() + "/marcical_out.cub";
  QString cubeFileName = "data/marci2isis/marcical_out1.cub";
  QVector<QString> args = {"from=data/marci2isis/K14_059003_3475_MA_00N112W_cropped.cub", "to=" + cubeFileName, "iof=no" };
  UserInterface options(APP_XML, args);
  marcical(options);

  Cube inCube("data/marci2isis/K14_059003_3475_MA_00N112W_cropped.cub");
  Cube outCube(cubeFileName);

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  std::unique_ptr<Histogram> inHist (inCube.histogram());


  EXPECT_EQ( inHist->Average(), outHist->Average() );
  EXPECT_EQ( inHist->Sum(), outHist->Sum() );
  EXPECT_EQ( inHist->ValidPixels(), outHist->ValidPixels() );
  EXPECT_EQ( inHist->StandardDeviation(), outHist->StandardDeviation() );
  

  /*
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);

  std::unique_ptr<Histogram> inHist (inCube.histogram());
  EXPECT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  EXPECT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  EXPECT_EQ(hist->ValidPixels(), 46);
  EXPECT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);
  */
}
