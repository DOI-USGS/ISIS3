#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "apollofindrx.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/apollofindrx.xml").expanded();

TEST_F(LargeCube, FunctionalTestApolloFindRxDefault) {

  PvlGroup reseaus("Reseaus");
  PvlKeyword samples = PvlKeyword("Sample", "200");
  samples += "400"; 
  samples += "600"; 
  samples += "800"; 

  PvlKeyword lines = PvlKeyword("Line", "200");
  lines += "400"; 
  lines += "600"; 
  lines += "800"; 

  PvlKeyword types = PvlKeyword("Type", "5");
  types += "5"; 
  types += "5"; 
  types += "5"; 

  PvlKeyword valid = PvlKeyword("Valid", "1");
  valid += "1"; 
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
  QVector<QString> args = {"tolerance=0.5", "patternsize=101", "deltax=2", "deltay=2"};

  UserInterface options(APP_XML, args);
  try {
    apollofindrx(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Call failed, Unable to process cube: " << e.what() << std::endl;
  }

  Pvl newLab = *testCube->label();

  PvlGroup newReseaus = newLab.findObject("IsisCube").findGroup("Reseaus");
  PvlKeyword testKeyword = newReseaus.findKeyword("Line");
  EXPECT_NEAR(testKeyword[0].toDouble(), 198.8, 0.0001);
  EXPECT_NEAR(testKeyword[1].toDouble(), 388.8, 0.0001);
  EXPECT_NEAR(testKeyword[2].toDouble(), 580.8, 0.0001);
  EXPECT_NEAR(testKeyword[3].toDouble(), 744.8, 0.0001);
   
  testKeyword = newReseaus.findKeyword("Sample");
  EXPECT_NEAR(testKeyword[0].toDouble(), 198.8, 0.0001);
  EXPECT_NEAR(testKeyword[1].toDouble(), 388.8, 0.0001);
  EXPECT_NEAR(testKeyword[2].toDouble(), 569.8, 0.0001);
  EXPECT_NEAR(testKeyword[3].toDouble(), 742.8, 0.0001);
  
  testKeyword = newReseaus.findKeyword("Valid");
  EXPECT_EQ(testKeyword[0].toInt(), 1);
  EXPECT_EQ(testKeyword[1].toInt(), 1);
  EXPECT_EQ(testKeyword[2].toInt(), 1);
  EXPECT_EQ(testKeyword[3].toInt(), 1);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, newReseaus.findKeyword("Status"), "Refined");

  // Assert some stuff
}
