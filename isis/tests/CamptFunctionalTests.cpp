#include <QTemporaryFile>

#include "campt.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

QString CAMPT_XML = FileName("$ISISROOT/bin/xml/campt.xml").expanded();

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


TEST_F(DefaultCube, FunctionalTestCamptFlatFileError) {
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

TEST_F(DefaultCube, FunctionalTestCamptDefaultParameters) {
  QVector<QString> args = {};
  UserInterface options(CAMPT_XML, args);
  Pvl appLog;

  campt(testCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("GroundPoint");

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), 602.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), 528.0);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, groundPoint.findKeyword("PixelValue"), "Null");
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("RightAscension"), 310.2070335306);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Declination"), -46.327246785573);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PlanetocentricLatitude"), 10.181441241544);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PlanetographicLatitude"), 10.299790241741);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveEast360Longitude"), 255.89292858176);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveEast180Longitude"), -104.10707141824);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveWest360Longitude"), 104.10707141824);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("PositiveWest180Longitude"), 104.10707141824);
  
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("BodyFixedCoordinate")[0]), -818.59644749774);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("BodyFixedCoordinate")[1]), -3257.2675597135);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("BodyFixedCoordinate")[2]),  603.17640797124);

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("LocalRadius"), 3412288.6569795);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SampleResolution"), 18.904248467739);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("LineResolution"), 18.904248467739);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("ObliqueDetectorResolution"), 19.336214219327);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("ObliquePixelResolution"), 19.336214219327);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("ObliqueLineResolution"), 19.336214219327);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("ObliqueSampleResolution"), 19.336214219327);

  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("SpacecraftPosition")[0]), -1152.8979327717);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("SpacecraftPosition")[1]), -3930.9421518203);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("SpacecraftPosition")[2]),  728.14118380775);

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SpacecraftAzimuth"), 240.08514246657);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SlantDistance"), 762.37204454685);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("TargetCenterDistance"), 4160.7294345949);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSpacecraftLatitude"), 10.078847382918);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSpacecraftLongitude"), 253.65422317887);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SpacecraftAltitude"), 753.22374841704);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("OffNadirAngle"), 9.9273765143684);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSpacecraftGroundAzimuth"), 267.5318718687);

  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("SunPosition")[0]),  147591102.63158);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("SunPosition")[1]), -127854342.1274);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("SunPosition")[2]), -81844199.02275);

  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSolarAzimuth"), 92.033828156965);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SolarDistance"), 1.4153000672557);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSolarLatitude"), -22.740326163641);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSolarLongitude"), 319.09846558533);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SubSolarGroundAzimuth"), 118.87356333938);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Phase"), 80.528381932125);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Incidence"), 70.127983116628);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Emission"), 12.133564327344);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("NorthAzimuth"), 332.65918493997);
  
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_TRUE( (QString) groundPoint.findKeyword("UTC") == "1977-07-09T20:05:51.5549999");
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("LocalSolarTime"), 7.7862975330952);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("SolarLongitude"), 294.73518830595);

  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionBodyFixed")[0]),  0.43850176257802);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionBodyFixed")[1]),  0.88365594846443);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionBodyFixed")[2]), -0.16391573737569);

  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionJ2000")[0]),  0.44577814515745);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionJ2000")[1]), -0.52737586689974);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionJ2000")[2]), -0.72329561059897);

  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionCamera")[0]), -1.27447324380581e-04);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionCamera")[1]),  2.5816511718707e-05);
  EXPECT_DOUBLE_EQ( toDouble(groundPoint.findKeyword("LookDirectionCamera")[2]),  0.99999999154535);
}
