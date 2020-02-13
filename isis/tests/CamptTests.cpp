#include <QTemporaryFile>

#include "campt.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

QString CAMPT_XML = FileName("$ISISROOT/bin/xml/campt.xml").expanded();

TEST_F(DefaultCube, BadColumnError) {
  // set up bad coordinates file
  std::ofstream of;
  QTemporaryFile badList;
  ASSERT_TRUE(badList.open());
  of.open(badList.fileName().toStdString());
  of << "1, 10,\n10,100,500\n100";
  of.close();

  // configure UserInterface arguments
  QVector<QString> args = {"to=output.pvl", "coordlist=" + badList.fileName(),
                           "coordtype=image"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  try {
    campt(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("Coordinate file formatted incorrectly."))
      << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected an IException with message: \"Coordinate file formatted incorrectly.\n"
              "Each row must have two columns: a sample,line or a latitude,longitude pair.\"";
  }
}


TEST_F(DefaultCube, FlatFileError) {
  // configure UserInterface arguments for flat file error
  QVector<QString> args = {"format=flat"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  try {
    campt(testCube, options, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("Flat file must have a name."))
      << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected an IException with message: \"Flat file must have a name.\"";
  }
}

TEST_F(DefaultCube, DefaultParameters) {
  QVector<QString> args = {};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, appLog.findKeyword("Filename"), ".cub");
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), 602.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), 528.0);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, appLog.findKeyword("PixelValue"), "Null");
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("RightAscension"), 310.2070335306);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Declination"), -46.327246785573);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PlanetocentricLatitude"), 10.181441241544);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PlanetographicLatitude"), 10.299790241741);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveEast360Longitude"), 255.89292858176);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveEast180Longitude"), -104.10707141824);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveWest360Longitude"), 104.10707141824);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveWest180Longitude"), 104.10707141824);

  std::cout<<appLog<<std::endl;
}
