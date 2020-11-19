#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "LineManager.h"
#include "Histogram.h"

#include "hicubecolormos.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/hicubenorm.xml").expanded();

TEST_F(MroCube, FunctionalTestsHicolormosDefault) {
  setInstrument("-74999", "HIRISE", "MARS RECONNAISSANCE ORBITER");

  QTemporaryDir prefix;
  QString outCubeFileName = "/tmp/outTEMP.cub";
  QVector<QString> args = {"to="+outCubeFileName};
  
  UserInterface options(APP_XML, args);
  try {
    hicolormos(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Unable to process HRISE image: " << e.what() << std::endl;
  }
}

