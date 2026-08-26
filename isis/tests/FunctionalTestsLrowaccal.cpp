#include <iostream>

#include <QString>
#include <QVector>
#include <QTemporaryDir>

#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "UserInterface.h"

#include "lrowaccal.h"

#include "gtest/gtest.h"

using namespace Isis;

static const QString APP_XML = FileName("$ISISROOT/bin/xml/lrowaccal.xml").expanded();

TEST(Lrowaccal, FunctionalTestLrowaccalRadianceUnitsLabelExists) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  const QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  const QString testCubeFileName = "data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub";

  QVector<QString> args = {"from=" + testCubeFileName,
                           "to=" + outCubeFileName,
                           "radiometrictype=Radiance",
                           "radiometricfile=Default"};
  UserInterface options(APP_XML, args);

  try {
    lrowaccal(options);
  }
  catch(const IException &e) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << e.what() << std::endl;
  }

  Cube outCube(outCubeFileName);

  ASSERT_TRUE(outCube.hasGroup("Radiometry"));
  PvlGroup &radiometry = outCube.group("Radiometry");

  ASSERT_TRUE(radiometry.hasKeyword("RadiometricType"));
  PvlKeyword &radiometricType = radiometry["RadiometricType"];
  ASSERT_EQ(radiometricType.unit().toStdString(), "W/m2/sr/um");
}

TEST(Lrowaccal, FunctionalTestLrowaccalRadianceUnitsLabelNotForIOF) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  const QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  const QString testCubeFileName = "data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub";

  QVector<QString> args = {"from=" + testCubeFileName,
                           "to=" + outCubeFileName,
                           "radiometrictype=IOF",
                           "radiometricfile=Default"};
  UserInterface options(APP_XML, args);

  try {
    lrowaccal(options);
  }
  catch(const IException &e) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << e.what() << std::endl;
  }

  Cube outCube(outCubeFileName);

  ASSERT_TRUE(outCube.hasGroup("Radiometry"));
  PvlGroup &radiometry = outCube.group("Radiometry");

  ASSERT_TRUE(radiometry.hasKeyword("RadiometricType"));
  PvlKeyword &radiometricType = radiometry["RadiometricType"];
  ASSERT_NE(radiometricType.unit().toStdString(), "W/m2/sr/um");
}
