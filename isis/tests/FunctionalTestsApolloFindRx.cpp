#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "apollofindrx.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/apollofindrx.xml").expanded();

TEST_F(SmallCube, FunctionalTestApolloFineRxDefault) {
  PvlGroup reseaus("Reseaus");
  PvlKeyword samples = PvlKeyword("Sample", "2");
  samples += "4"; 
  samples += "6"; 
  samples += "8";

  PvlKeyword lines = PvlKeyword("Line", "2");
  lines += "4"; 
  lines += "6"; 
  lines += "8";

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

  PvlGroup &inst = testCube->label()->findObject("IsisCube").findGroup("Instrument");
  inst.findKeyword("SpacecraftName").setValue("APOLLO 15");
  inst.findKeyword("InstrumentId").setValue("METRIC"); 
  
  std::cout << *lab << std::endl;

  QTemporaryDir prefix;
  QString outCubeFileName = prefix.path() + "/outTemp.cub";
  QVector<QString> args = {"to="+outCubeFileName, "tolerance=0.125"};

  UserInterface options(APP_XML, args);
  try {
    apollofindrx(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Call failed, Unable to process cube: " << e.what() << std::endl;
  }
  
  // Assert some stuff
}