#include <QTemporaryDir>

#include "hicubeit.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/hicubeit.xml").expanded();

TEST(Hicubeit, Default) {
  QTemporaryDir prefix;
  QString outFileName = prefix.path() + "/out.cub";
  QVector<QString> args = { "BG= data/hicubeit/BG.cub",
                            "IR= data/hicubeit/IR.cub",
                            "RE= data/hicubeit/RE.cub",
                            "TO=" + outFileName };
  UserInterface options(APP_XML, args);
  Pvl appLog;

  try {
    hicubeit(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  } 

  Cube outCube(outFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  PvlGroup bandbin = outLabel->findGroup("BandBin", Pvl::Traverse);

  EXPECT_EQ(bandbin["Name"][0], "NearInfrared");
  EXPECT_EQ(bandbin["Name"][1], "Red");
  EXPECT_EQ(bandbin["Name"][2], "BlueGreen");

  EXPECT_EQ(int(dimensions["Samples"]), 2048);
  EXPECT_EQ(int(dimensions["Lines"]), 10);
  EXPECT_EQ(int(dimensions["Bands"]), 3);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  EXPECT_NEAR(hist->Average(), 0.960096, .00001);
  EXPECT_NEAR(hist->Sum(), 9831.38, .01);
  EXPECT_EQ(hist->ValidPixels(), 10240);
  EXPECT_NEAR(hist->StandardDeviation(), 0.0626452, .00001);
}
