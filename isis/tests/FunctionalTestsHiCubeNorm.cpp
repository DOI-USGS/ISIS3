#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "LineManager.h"

#include "hicubenorm.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/hicubenorm.xml").expanded();

TEST_F(MroCube, FunctionalTestsHiCubeNorm) {
  setInstrument("-74999", "HIRISE", "MARS RECONNAISSANCE ORBITER");

  QTemporaryDir prefix;
  QString outCubeFileName = "/tmp/outTEMP2.cub";
  
  QVector<QString> args = {"to="+outCubeFileName, "stats=/tmp/stats.csv", "HIGHPASS_MODE=HIGHPASS_SUBTRACT"};
 if (QFile::exists("/tmp/test.cub")) {
    QFile::remove("/tmp/test.cub");
  } 
  
  QFile::copy(testCube->fileName(), "/tmp/test.cub");

  std::cout << args.at(0) << std::endl;
  UserInterface options(APP_XML, args);
  try {
    hicubenorm(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Unable to process HRISE image: " << e.what() << std::endl;
  }

  
  // Assert some stuff
}
