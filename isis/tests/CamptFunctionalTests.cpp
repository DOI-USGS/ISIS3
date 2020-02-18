#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>

#include "campt.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/campt.xml").expanded();

TEST_F(DefaultCube, FunctionalTestCamptBadColumnError) {
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
  UserInterface options(APP_XML, args);
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


TEST_F(DefaultCube, FunctionalTestCamptFlatFileError) {
  // configure UserInterface arguments for flat file error
  QVector<QString> args = {"format=flat"};
  UserInterface options(APP_XML, args);
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

TEST_F(DefaultCube, FunctionalTestCamptDefault) {
  QVector<QString> args = {};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), 602.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), 528.0);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, groundPoint.findKeyword("PixelValue"), "Null");
  EXPECT_NEAR( (double) groundPoint.findKeyword("RightAscension"), 310.2070335306, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("Declination"), -46.327246785573, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("PlanetocentricLatitude"), 10.181441241544, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("PlanetographicLatitude"), 10.299790241741, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("PositiveEast360Longitude"), 255.89292858176, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("PositiveEast180Longitude"), -104.10707141824, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("PositiveWest360Longitude"), 104.10707141824, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("PositiveWest180Longitude"), 104.10707141824, 1e-8);

  EXPECT_NEAR( toDouble(groundPoint.findKeyword("BodyFixedCoordinate")[0]), -818.59644749774, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("BodyFixedCoordinate")[1]), -3257.2675597135, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("BodyFixedCoordinate")[2]),  603.17640797124, 1e-8);

  EXPECT_NEAR( (double) groundPoint.findKeyword("LocalRadius"), 3412288.6569795, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SampleResolution"), 18.904248467739, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("LineResolution"), 18.904248467739, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("ObliqueDetectorResolution"), 19.336214219327, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("ObliquePixelResolution"), 19.336214219327, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("ObliqueLineResolution"), 19.336214219327, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("ObliqueSampleResolution"), 19.336214219327, 1e-8);

  EXPECT_NEAR( toDouble(groundPoint.findKeyword("SpacecraftPosition")[0]), -1152.8979327717, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("SpacecraftPosition")[1]), -3930.9421518203, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("SpacecraftPosition")[2]),  728.14118380775, 1e-8);

  EXPECT_NEAR( (double) groundPoint.findKeyword("SpacecraftAzimuth"), 240.08514246657, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SlantDistance"), 762.37204454685, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("TargetCenterDistance"), 4160.7294345949, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSpacecraftLatitude"), 10.078847382918, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSpacecraftLongitude"), 253.65422317887, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SpacecraftAltitude"), 753.22374841704, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("OffNadirAngle"), 9.9273765143684, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSpacecraftGroundAzimuth"), 267.5318718687, 1e-8);

  EXPECT_NEAR( toDouble(groundPoint.findKeyword("SunPosition")[0]),  147591102.63158, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("SunPosition")[1]), -127854342.1274, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("SunPosition")[2]), -81844199.02275, 1e-8);

  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSolarAzimuth"), 92.033828156965, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SolarDistance"), 1.4153000672557, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSolarLatitude"), -22.740326163641, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSolarLongitude"), 319.09846558533, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SubSolarGroundAzimuth"), 118.87356333938, 1e-8);

  EXPECT_NEAR( (double) groundPoint.findKeyword("Phase"), 80.528381932125, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("Incidence"), 70.127983116628, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("Emission"), 12.133564327344, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("NorthAzimuth"), 332.65918493997, 1e-8);

  EXPECT_NEAR( (double) groundPoint.findKeyword("EphemerisTime"), -709401200.26114, 1e-8);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, groundPoint.findKeyword("UTC"), "1977-07-09T20:05:51.5549999");
  EXPECT_NEAR( (double) groundPoint.findKeyword("LocalSolarTime"), 7.7862975330952, 1e-8);
  EXPECT_NEAR( (double) groundPoint.findKeyword("SolarLongitude"), 294.73518830595, 1e-8);

  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionBodyFixed")[0]),  0.43850176257802, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionBodyFixed")[1]),  0.88365594846443, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionBodyFixed")[2]), -0.16391573737569, 1e-8);

  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionJ2000")[0]),  0.44577814515745, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionJ2000")[1]), -0.52737586689974, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionJ2000")[2]), -0.72329561059897, 1e-8);

  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionCamera")[0]), -1.27447324380581e-04, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionCamera")[1]),  2.5816511718707e-05, 1e-8);
  EXPECT_NEAR( toDouble(groundPoint.findKeyword("LookDirectionCamera")[2]),  0.99999999154535, 1e-8);
}

TEST_F(DefaultCube, FunctionalTestCamptSetSL) {
  QVector<QString> args = {"sample=25.0", "line=25.0"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), 25.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), 25.0);
}

TEST_F(DefaultCube, FunctionalTestCamptSetS) {
  QVector<QString> args = {"sample=25.0"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), 25.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), 528.0);
}

TEST_F(DefaultCube, FunctionalTestCamptSetL) {
  QVector<QString> args = {"line=25.0"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), 602.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), 25.0);
}

TEST_F(DefaultCube, FunctionalTestCamptSetGround) {
  QVector<QString> args = {"type=ground", "latitude=10.181441241544", "longitude=255.89292858176"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_NEAR( (double) groundPoint.findKeyword("Sample"), 602.0, 1e-4);
  EXPECT_NEAR( (double) groundPoint.findKeyword("Line"), 528.0, 1e-4);
}


TEST_F(DefaultCube, FunctionalTestCamptFlat) {
  // Setup output file
  QTemporaryFile flatFile;
  flatFile.open();

  QVector<QString> args = {"format=flat", "to=" + flatFile.fileName(), "append=false"};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);

  int lineNumber = 0;
  QTextStream flatStream(&flatFile);
  while(!flatStream.atEnd()) {
    QString line = flatStream.readLine();
    QStringList fields = line.split(",");

    if(lineNumber == 0) {
      EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(1), "Sample");
      EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(2), "Line");
    }
    else if(lineNumber == 1) {
      EXPECT_DOUBLE_EQ(fields.value(1).toDouble(), 602.0);
      EXPECT_DOUBLE_EQ(fields.value(2).toDouble(), 528.0);
    }
    lineNumber++;
  }
}