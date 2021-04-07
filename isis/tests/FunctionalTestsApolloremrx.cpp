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

  testCube->label()->findObject("IsisCube").findGroup("Reseaus")["Status"] = "Refined";

  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path()+"/outTEMP.cub";
  QVector<QString> args = {"to=" + outCubeFileName,"action=null"};

  UserInterface options(APP_XML, args);
  try {
    apolloremrx(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Call failed, Unable to process cube: " << e.what() << std::endl;
  }

  Pvl newLab = *testCube->label();

  PvlGroup newReseaus = newLab.findObject("IsisCube").findGroup("Reseaus");
  PvlKeyword testKeyword = newReseaus.findKeyword("Line");
  EXPECT_NEAR(testKeyword[0].toDouble(), 200, 0.0001);
  EXPECT_NEAR(testKeyword[1].toDouble(), 400, 0.0001);
  EXPECT_NEAR(testKeyword[2].toDouble(), 600, 0.0001);

  testKeyword = newReseaus.findKeyword("Sample");
  EXPECT_NEAR(testKeyword[0].toDouble(), 200, 0.0001);
  EXPECT_NEAR(testKeyword[1].toDouble(), 400, 0.0001);
  EXPECT_NEAR(testKeyword[2].toDouble(), 600, 0.0001);

  testKeyword = newReseaus.findKeyword("Valid");
  EXPECT_EQ(testKeyword[0].toInt(), 1);
  EXPECT_EQ(testKeyword[1].toInt(), 1);
  EXPECT_EQ(testKeyword[2].toInt(), 1);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, newReseaus.findKeyword("Status"), "Removed");

  std::unique_ptr<Histogram> hist (testCube->histogram());

  ASSERT_DOUBLE_EQ(hist->Average(), 11449.5);
  ASSERT_DOUBLE_EQ(hist->Sum(), 6004232295000);
  ASSERT_EQ(hist->ValidPixels(), 524410000);
  ASSERT_NEAR(hist->StandardDeviation(), 6610.66055, 0.00001);
}

TEST_F(ApolloCube, FunctionalTestApolloremrxRemovedError) {

  testCube->label()->findObject("IsisCube").findGroup("Reseaus")["Status"] = "Removed";

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

  testCube->label()->findObject("IsisCube").findGroup("Instrument")["SpacecraftName"] = "Galileo Orbiter";

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
