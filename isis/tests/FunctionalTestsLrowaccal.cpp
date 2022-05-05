#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "lrowaccal.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/lrowaccal.xml").expanded();

TEST(Lrowaccal, FunctionalTestLrowaccalRadianceUnitsLabelExists) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());
  
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QString testCubeFileName = "data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub";

  QVector<QString> args = {"from=" + testCubeFileName, 
                           "to=" + outCubeFileName, 
                           "radiometrictype=Radiance", 
                           "radiometricfile=Default"};
  UserInterface options(APP_XML, args);

  try {
    lrowaccal(options);
  }
  catch(IException &e) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << e.what() << std::endl;
  }

  Cube outCube(outCubeFileName);
  Pvl *outCubeLabel = outCube.label();

  PvlGroup radiometryGroup;
  try {
    radiometryGroup = outCubeLabel->findGroup("Radiometry", Pvl::Traverse);
  }
  catch(IException &e) {
    FAIL() << "Unable to find Radiometry group in output cube label: " << e.what() << std::endl;
  }

  PvlKeyword radiometricTypeKeyword;
  try {
    radiometricTypeKeyword = radiometryGroup.findKeyword("RadiometricType");
  }
  catch(IException &e) {
    FAIL() << "Unable to find RadiometricType keyword in Radiometry group of output cube label: " << e.what() << std::endl;
  }
  EXPECT_EQ(radiometricTypeKeyword.unit().toStdString(), "W/m2/sr/um");
}

TEST(Lrowaccal, FunctionalTestLrowaccalRadianceUnitsLabelNotForIOF) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());
  
  QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QString testCubeFileName = "data/lrowaccal/M1388981421CE.tmp.vis.even.reduced.cub";

  QVector<QString> args = {"from=" + testCubeFileName, 
                           "to=" + outCubeFileName, 
                           "radiometrictype=IOF", 
                           "radiometricfile=Default"};
  UserInterface options(APP_XML, args);

  try {
    lrowaccal(options);
  }
  catch(IException &e) {
    FAIL() << "Call to lrowaccal failed, unable to calibrate cube: " << e.what() << std::endl;
  }

  Cube outCube(outCubeFileName);
  Pvl *outCubeLabel = outCube.label();

  PvlGroup radiometryGroup;
  try {
    radiometryGroup = outCubeLabel->findGroup("Radiometry", Pvl::Traverse);
  }
  catch(IException &e) {
    FAIL() << "Unable to find Radiometry group in output cube label: " << e.what() << std::endl;
  }

  PvlKeyword radiometricTypeKeyword;
  try {
    radiometricTypeKeyword = radiometryGroup.findKeyword("RadiometricType");
  }
  catch(IException &e) {
    FAIL() << "Unable to find RadiometricType keyword in Radiometry group of output cube label: " << e.what() << std::endl;
  }
  EXPECT_NE(radiometricTypeKeyword.unit().toStdString(), "W/m2/sr/um");
}
