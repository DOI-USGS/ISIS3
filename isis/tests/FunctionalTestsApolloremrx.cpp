#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "apolloremrx.h"

#include "gtest/gtest.h"

using namespace Isis;
using ::testing::HasSubstr;

static QString APP_XML = FileName("$ISISROOT/bin/xml/apolloremrx.xml").expanded();

TEST_F(ApolloCube, FunctionalTestApolloremrxDefault) {

  testCube->group("RESEAUS")["STATUS"] = "Refined";

  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path()+"/outTEMP.cub";
  QVector<QString> args = {"to=" + outCubeFileName, "action=null"};

  UserInterface options(APP_XML, args);
  try {
    apolloremrx(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Call failed, Unable to process cube: " << e.what() << std::endl;
  }

  Cube cube(outCubeFileName);
  Pvl *isisLabel = cube.label();

  PvlGroup reseaus = isisLabel->findObject("IsisCube").findGroup("Reseaus");
  PvlKeyword lineKey = reseaus.findKeyword("Line");
  EXPECT_NEAR(lineKey[0].toDouble(), 200, 0.0001);
  EXPECT_NEAR(lineKey[1].toDouble(), 400, 0.0001);
  EXPECT_NEAR(lineKey[2].toDouble(), 600, 0.0001);

  PvlKeyword sampleKey = reseaus.findKeyword("Sample");
  EXPECT_NEAR(sampleKey[0].toDouble(), 200, 0.0001);
  EXPECT_NEAR(sampleKey[1].toDouble(), 400, 0.0001);
  EXPECT_NEAR(sampleKey[2].toDouble(), 600, 0.0001);

  PvlKeyword validKey = reseaus.findKeyword("Valid");
  EXPECT_EQ(validKey[0].toInt(), 1);
  EXPECT_EQ(validKey[1].toInt(), 1);
  EXPECT_EQ(validKey[2].toInt(), 1);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, reseaus.findKeyword("Status"), "Removed");

  std::unique_ptr<Histogram> hist (testCube->histogram());

  ASSERT_DOUBLE_EQ(hist->Average(), 11449.5);
  ASSERT_DOUBLE_EQ(hist->Sum(), 6004232295000);
  ASSERT_EQ(hist->ValidPixels(), 524410000);
  ASSERT_NEAR(hist->StandardDeviation(), 6610.66055, 0.00001);
}

TEST_F(ApolloCube, FunctionalTestApolloremrxRemovedError) {

  testCube->group("RESEAUS")["STATUS"] = "Removed";

  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path()+"/outTEMP.cub";
  QVector<QString> args = {"to=" + outCubeFileName,"action=null"};

  UserInterface options(APP_XML, args);
  try {
    apolloremrx(testCube, options);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("appears to already have reseaus removed."));
  }
}

TEST_F(ApolloCube, FunctionalTestApolloremrxSpacecraftError) {

  testCube->group("Instrument")["SpacecraftName"] = "Galileo Orbiter";

  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path()+"/outTEMP.cub";
  QVector<QString> args = {"to=" + outCubeFileName,"action=null"};

  UserInterface options(APP_XML, args);
  try {
    apolloremrx(testCube, options);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("This application is for use with Apollo spacecrafts only."));
  }
}

TEST_F(ApolloCube, FunctionalTestApolloremrxNominalError) {

  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path()+"/outTEMP.cub";
  QVector<QString> args = {"to=" + outCubeFileName,"action=null"};

  UserInterface options(APP_XML, args);
  try {
    apolloremrx(testCube, options);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("appears to have nominal reseau status. You must run findrx first"));
  }
}
