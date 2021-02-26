#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "moccal.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/moccal.xml").expanded();

TEST_F(MgsMocCube, FunctionalTestMroMoccalDefault) {
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"to="+outCubeFileName};
  
  UserInterface options(APP_XML, args);

  try {
    moccal(testCube.get(), options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  
  // Assert some stuff
}