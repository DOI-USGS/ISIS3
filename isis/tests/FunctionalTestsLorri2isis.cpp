#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "lorri2isis.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/lorri2isis.xml").expanded();

TEST(Lorri2Isis, Lorri2IsisTestDefault) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lorri2isisTEMP.cub";
  QVector<QString> args = {"from=data/lorri2isis/",  "to="+ cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    lorri2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LORRI image: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();



  // Assert some stuff
}
