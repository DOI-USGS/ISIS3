#include <QTextStream>
#include <QStringList>
#include <QTemporaryFile>

#include "Fixtures.h"
#include "Camera.h"
#include "CameraFixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "XmlToPvlTranslationManager.h"
#include "UserInterface.h"

#include "pixel2map.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString PIXEL2MAP_XML = FileName("$ISISROOT/bin/xml/pixel2map.xml").expanded();


TEST_F(DefaultCube, FunctionalTestPixel2mapVector) {
  QFile csvFile(tempDir.path() + "/vect.csv");
  QFile vrtFile(tempDir.path() + "/vect.vrt");
  //QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"tovect="+csvFile.fileName(), "from="+ testCube->fileName()};

  UserInterface options(PIXEL2MAP_XML, args);

  try {
    pixel2map(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  // TEST 1: Check we have both csv and vrt output files
  
  FileName csvFileOut(tempDir.path() + "/vect.csv");
  EXPECT_TRUE(csvFileOut.fileExists());
  FileName vrtFileOut(tempDir.path() + "/vect.vrt");
  EXPECT_TRUE(vrtFileOut.fileExists());
  
}
