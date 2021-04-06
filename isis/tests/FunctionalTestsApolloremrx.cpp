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

TEST_F(LargeCube, FunctionalTestApolloremrxDefault) {

  PvlGroup reseaus("Reseaus");
  PvlKeyword samples = PvlKeyword("Sample", "100.8141");
  samples += "192.8";
  samples += "167.8";

  PvlKeyword lines = PvlKeyword("Line",  "100.8141");
  lines += "192.8";
  lines += "275.8";

  PvlKeyword types = PvlKeyword("Type", "5");
  types += "5";
  types += "5";

  PvlKeyword valid = PvlKeyword("Valid", "1");
  valid += "1";
  valid += "1";

  reseaus += lines;
  reseaus += samples;
  reseaus += types;
  reseaus += valid;
  reseaus += PvlKeyword("Status", "Refined");

  std::istringstream instStr (R"(
    Group = Instrument
        SpacecraftName = "APOLLO 15"
        InstrumentId   = METRIC
        TargetName     = MOON
        StartTime      = 1971-08-01T14:58:03.78
    End_Group
  )");

  PvlGroup instGroup;
  instStr >> instGroup;

  Pvl *lab = testCube->label();
  lab->findObject("IsisCube").addGroup(reseaus);
  lab->findObject("IsisCube").addGroup(instGroup);

  testCube->reopen("r");

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
  EXPECT_NEAR(testKeyword[0].toDouble(), 100.8141, 0.0001);
  EXPECT_NEAR(testKeyword[1].toDouble(), 192.8, 0.0001);
  EXPECT_NEAR(testKeyword[2].toDouble(), 275.8, 0.0001);

  testKeyword = newReseaus.findKeyword("Sample");
  EXPECT_NEAR(testKeyword[0].toDouble(), 100.8141, 0.0001);
  EXPECT_NEAR(testKeyword[1].toDouble(), 192.8, 0.0001);
  EXPECT_NEAR(testKeyword[2].toDouble(), 167.8, 0.0001);

  testKeyword = newReseaus.findKeyword("Valid");
  EXPECT_EQ(testKeyword[0].toInt(), 1);
  EXPECT_EQ(testKeyword[1].toInt(), 1);
  EXPECT_EQ(testKeyword[2].toInt(), 1);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, newReseaus.findKeyword("Status"), "Removed");

  std::unique_ptr<Histogram> hist (testCube->histogram());

  ASSERT_DOUBLE_EQ(hist->Average(), 499.5);
  ASSERT_DOUBLE_EQ(hist->Sum(), 499500000);
  ASSERT_EQ(hist->ValidPixels(), 1000000);
  ASSERT_NEAR(hist->StandardDeviation(), 288.67513, 0.00001);
}

TEST_F(LargeCube, FunctionalTestApolloremrxRemovedError) {

  PvlGroup reseaus("Reseaus");
  PvlKeyword samples = PvlKeyword("Sample", "100.8141");
  samples += "192.8";
  samples += "167.8";

  PvlKeyword lines = PvlKeyword("Line",  "100.8141");
  lines += "192.8";
  lines += "275.8";

  PvlKeyword types = PvlKeyword("Type", "5");
  types += "5";
  types += "5";

  PvlKeyword valid = PvlKeyword("Valid", "1");
  valid += "1";
  valid += "1";

  reseaus += lines;
  reseaus += samples;
  reseaus += types;
  reseaus += valid;
  reseaus += PvlKeyword("Status", "Removed");

  std::istringstream instStr (R"(
    Group = Instrument
        SpacecraftName = "APOLLO 15"
        InstrumentId   = METRIC
        TargetName     = MOON
        StartTime      = 1971-08-01T14:58:03.78
    End_Group
  )");

  PvlGroup instGroup;
  instStr >> instGroup;

  Pvl *lab = testCube->label();
  lab->findObject("IsisCube").addGroup(reseaus);
  lab->findObject("IsisCube").addGroup(instGroup);

  testCube->reopen("r");

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

TEST_F(LargeCube, FunctionalTestApolloremrxSpacecraftError) {

  PvlGroup reseaus("Reseaus");
  PvlKeyword samples = PvlKeyword("Sample", "100.8141");
  samples += "192.8";
  samples += "167.8";

  PvlKeyword lines = PvlKeyword("Line",  "100.8141");
  lines += "192.8";
  lines += "275.8";

  PvlKeyword types = PvlKeyword("Type", "5");
  types += "5";
  types += "5";

  PvlKeyword valid = PvlKeyword("Valid", "1");
  valid += "1";
  valid += "1";

  reseaus += lines;
  reseaus += samples;
  reseaus += types;
  reseaus += valid;
  reseaus += PvlKeyword("Status", "Refined");

  std::istringstream instStr (R"(
    Group = Instrument
        SpacecraftName = "Galileo Orbiter"
        InstrumentId   = METRIC
        TargetName     = MOON
        StartTime      = 1971-08-01T14:58:03.78
    End_Group
  )");

  PvlGroup instGroup;
  instStr >> instGroup;

  Pvl *lab = testCube->label();
  lab->findObject("IsisCube").addGroup(reseaus);
  lab->findObject("IsisCube").addGroup(instGroup);

  testCube->reopen("r");

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

TEST_F(LargeCube, FunctionalTestApolloremrxNominalError) {

  PvlGroup reseaus("Reseaus");
  PvlKeyword samples = PvlKeyword("Sample", "100.8141");
  samples += "192.8";
  samples += "167.8";

  PvlKeyword lines = PvlKeyword("Line",  "100.8141");
  lines += "192.8";
  lines += "275.8";

  PvlKeyword types = PvlKeyword("Type", "5");
  types += "5";
  types += "5";

  PvlKeyword valid = PvlKeyword("Valid", "1");
  valid += "1";
  valid += "1";

  reseaus += lines;
  reseaus += samples;
  reseaus += types;
  reseaus += valid;
  reseaus += PvlKeyword("Status", "Nominal");

  std::istringstream instStr (R"(
    Group = Instrument
        SpacecraftName = "APOLLO 15"
        InstrumentId   = METRIC
        TargetName     = MOON
        StartTime      = 1971-08-01T14:58:03.78
    End_Group
  )");

  PvlGroup instGroup;
  instStr >> instGroup;

  Pvl *lab = testCube->label();
  lab->findObject("IsisCube").addGroup(reseaus);
  lab->findObject("IsisCube").addGroup(instGroup);

  testCube->reopen("r");

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
