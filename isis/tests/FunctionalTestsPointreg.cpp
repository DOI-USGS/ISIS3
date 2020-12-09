#include "pointreg.h"

#include "Fixtures.h"
#include "TestUtilities.h"
#include "UserInterface.h"
#include "Histogram.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/pointreg.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestPointRegDefault) {
  // Setup def file
  QTemporaryFile defFile("XXXXXX.def");
  ASSERT_TRUE(defFile.open());

  std::ofstream ofstream;
  ofstream.open(defFile.fileName().toStdString());
  ofstream << "Object = Filters\n\tGroup = Cube_NumPoints\n\t\tGreaterThan = 15\n\tEndGroup\nEndObject";
  ofstream.close();

  // Setup output flat file
  QTemporaryFile flatFile;
  ASSERT_TRUE(flatFile.open());

  Pvl *log;
  QTemporaryDir prefix;
  QString outNetPath = prefix.path() + "/outNet.net";
  QVector<QString> args = { "fromlist=" + cubeList->fileName(),
                            "deffile=" + defFile->fileName(),
                            "flatfile=" + flatFile->fileName(),
                            "onet=" + outNetPath};
  UserInterface options(APP_XML, args);
  try {
    pointreg(network, options, &log);
  }
  catch (IException &e) {
    FAIL() << "Unable to register " << e.toString().toStdString().c_str() << std::endl;
  }
}
