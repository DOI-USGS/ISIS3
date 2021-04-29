#include <QTemporaryDir>

#include "marciflip.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/marciflip.xml").expanded();

TEST(Marciflip, MarciflipTestDefault) {
  QTemporaryDir prefix;
  //QString cubeFileName = prefix.path() + "/marciflip_out.cub"; TODO
  QString cubeFileName = "/Users/tgiroux/Desktop/marciflip_out.cub";
  QVector<QString> args = {"from=data/marci2isis/K14_059003_3475_MA_00N112W_cropped.cub", "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  marciflip(options);

  Cube outCube(cubeFileName);
  Pvl *label = outCube.label();

  PvlGroup &dims = label->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_EQ( (int)dims["Lines"], 2 );
  EXPECT_EQ( (int)dims["Samples"], 50 );
  EXPECT_EQ( (int)dims["Bands"], 5 );

  PvlGroup &inst = label->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ( inst["VariableExposureDuration"][0].toStdString(), "8.8" );

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  EXPECT_NEAR( outHist->Average(), 0.034876, 1e-6 );
  EXPECT_NEAR( outHist->Sum(), 1.39504, 1e-5 );
  EXPECT_EQ( outHist->ValidPixels(), 40 );
  EXPECT_NEAR( outHist->StandardDeviation(), 0.0110666, 1e-7 );
}