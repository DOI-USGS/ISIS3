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
  QString cubeFileName = prefix.path() + "/marciflip_out.cub";
  QVector<QString> args = {"from=data/marci2isis/T02_001002_1200_MC_00N284W_cropped.cub", "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  marciflip(options);

  Cube outCube(cubeFileName);
  Pvl *label = outCube.label();

  PvlGroup &dims = label->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_EQ( (int)dims["Lines"], 48 );
  EXPECT_EQ( (int)dims["Samples"], 2 );
  EXPECT_EQ( (int)dims["Bands"], 3 );

  PvlGroup &inst = label->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ( (int)inst.findKeyword("DataFlipped"), 0 );

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  EXPECT_NEAR( outHist->Average(), 50.552, 1e-3 );
  EXPECT_NEAR( outHist->Sum(), 4853, 1 );
  EXPECT_EQ( outHist->ValidPixels(), 96 );
  EXPECT_NEAR( outHist->StandardDeviation(), 11.63829, 1e-5 );

}