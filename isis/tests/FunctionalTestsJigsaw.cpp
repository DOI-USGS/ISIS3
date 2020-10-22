#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "jigsaw.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/jigsaw.xml").expanded();

TEST_F(StereoPair, FunctionalTestJigsawCamSolveAll) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName, 
                           "observations=yes", "update=yes", "Cksolvedegree=3", "OUTLIER_REJECTION=yes", 
                           "Camsolve=all", "twist=no", "Spsolve=none", "Radius=no", "Residuals_csv=off",  "SIGMA0=999"};

  UserInterface options(APP_XML, args);
    
  std::cout <<  cube1->camera() << " " << cube2->camera() << std::endl;
  
  Pvl log; 
  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  
  // Assert some stuff
}