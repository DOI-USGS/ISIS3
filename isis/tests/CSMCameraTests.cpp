#include <QString>
#include <iostream>

#include "csm/csm.h"
#include "csm/Ellipsoid.h"

#include "Fixtures.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MockCsmPlugin.h"
#include "Mocks.h"
#include "TestUtilities.h"
#include "StringBlob.h"
#include "FileName.h"
#include "Fixtures.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gmock/gmock.h"

using namespace Isis;

TEST_F(CSMCameraFixture, SetImage) {
  csm::Ellipsoid wgs84;
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus(MatchImageCoord(csm::ImageCoord(4.5, 4.5)), ::testing::_, ::testing::_, ::testing::_))
      .Times(1)
      // looking straight down X-Axis
      .WillOnce(::testing::Return(csm::EcefLocus(wgs84.getSemiMajorRadius() + 50000, 0, 0, -1, 0, 0)));
  EXPECT_CALL(mockModel, getImageTime)
      .Times(1)
      .WillOnce(::testing::Return(10.0));

  EXPECT_TRUE(testCam->SetImage(5, 5));
  EXPECT_EQ(testCam->UniversalLatitude(), 0.0);
  EXPECT_EQ(testCam->UniversalLongitude(), 0.0);
  EXPECT_THAT(testCam->lookDirectionBodyFixed(), ::testing::ElementsAre(-1.0, 0.0, 0.0));

  iTime refTime("2000-01-01T11:58:55.816");
  EXPECT_EQ((refTime + 10.0).Et(), testCam->time().Et());
}


TEST_F(CSMCameraDemFixture, SetImage) {
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus(MatchImageCoord(csm::ImageCoord(4.5, 4.5)), ::testing::_, ::testing::_, ::testing::_))
      .Times(1)
      // looking straight down X-Axis
      .WillOnce(::testing::Return(csm::EcefLocus(demRadius + 50000, 0, 0, -1, 0, 0)));
  EXPECT_CALL(mockModel, computeGroundPartials)
      .WillRepeatedly(::testing::Return(std::vector<double>{1, 2, 3, 4, 5, 6}));
  EXPECT_CALL(mockModel, getImageTime)
      .Times(1)
      .WillOnce(::testing::Return(10.0));

  testCam->SetImage(5, 5);
  EXPECT_EQ(testCam->UniversalLatitude(), 0.0);
  EXPECT_EQ(testCam->UniversalLongitude(), 0.0);
}


TEST_F(CSMCameraFixture, SetGround) {
  // Define some things to match/return
  csm::Ellipsoid wgs84;
  csm::ImageCoord imagePt(4.5, 4.5);
  csm::EcefCoord groundPt(wgs84.getSemiMajorRadius(), 0, 0);
  csm::EcefLocus imageLocus(wgs84.getSemiMajorRadius() + 50000, 0, 0, -1, 0, 0);

  // Setup expected calls/returns
  EXPECT_CALL(mockModel, groundToImage(MatchEcefCoord(groundPt), ::testing::_, ::testing::_, ::testing::_))
      .Times(4)
      .WillRepeatedly(::testing::Return(imagePt));
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus(MatchImageCoord(imagePt), ::testing::_, ::testing::_, ::testing::_))
      .Times(4)
      .WillRepeatedly(::testing::Return(imageLocus));
  EXPECT_CALL(mockModel, getImageTime)
      .Times(4)
      .WillRepeatedly(::testing::Return(10.0));

  iTime refTime("2000-01-01T11:58:55.816");

  EXPECT_TRUE(testCam->SetGround(Latitude(0.0, Angle::Degrees), Longitude(0.0, Angle::Degrees)));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);
  EXPECT_EQ((refTime + 10.0).Et(), testCam->time().Et());
  EXPECT_THAT(testCam->lookDirectionBodyFixed(), ::testing::ElementsAre(-1.0, 0.0, 0.0));

  EXPECT_TRUE(testCam->SetGround(SurfacePoint(Latitude(0.0, Angle::Degrees),
                                 Longitude(0.0, Angle::Degrees),
                                 Distance(wgs84.getSemiMajorRadius(), Distance::Meters))));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);
  EXPECT_EQ((refTime + 10.0).Et(), testCam->time().Et());
  EXPECT_THAT(testCam->lookDirectionBodyFixed(), ::testing::ElementsAre(-1.0, 0.0, 0.0));

  EXPECT_TRUE(testCam->SetUniversalGround(0.0, 0.0));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);
  EXPECT_EQ((refTime + 10.0).Et(), testCam->time().Et());
  EXPECT_THAT(testCam->lookDirectionBodyFixed(), ::testing::ElementsAre(-1.0, 0.0, 0.0));

  EXPECT_TRUE(testCam->SetUniversalGround(0.0, 0.0, wgs84.getSemiMajorRadius()));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);
  EXPECT_EQ((refTime + 10.0).Et(), testCam->time().Et());
  EXPECT_THAT(testCam->lookDirectionBodyFixed(), ::testing::ElementsAre(-1.0, 0.0, 0.0));
}


TEST_F(CSMCameraDemFixture, SetGround) {
  // Define some things to match/return
  csm::ImageCoord imagePt(4.5, 4.5);
  csm::EcefCoord groundPt(demRadius, 0, 0);
  csm::EcefLocus imageLocus(demRadius + 50000, 0, 0, -1, 0, 0);

  // Setup expected calls/returns
  EXPECT_CALL(mockModel, groundToImage(MatchEcefCoord(groundPt), ::testing::_, ::testing::_, ::testing::_))
      .Times(4)
      .WillRepeatedly(::testing::Return(imagePt));
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus(MatchImageCoord(imagePt), ::testing::_, ::testing::_, ::testing::_))
      .Times(4)
      .WillRepeatedly(::testing::Return(imageLocus));
  EXPECT_CALL(mockModel, getImageTime)
      .Times(4)
      .WillRepeatedly(::testing::Return(10.0));

  EXPECT_TRUE(testCam->SetGround(Latitude(0.0, Angle::Degrees), Longitude(0.0, Angle::Degrees)));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);

  EXPECT_TRUE(testCam->SetGround(SurfacePoint(Latitude(0.0, Angle::Degrees),
                                 Longitude(0.0, Angle::Degrees),
                                 Distance(demRadius, Distance::Meters))));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);

  EXPECT_TRUE(testCam->SetUniversalGround(0.0, 0.0));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);

  EXPECT_TRUE(testCam->SetUniversalGround(0.0, 0.0, demRadius));
  EXPECT_EQ(testCam->Line(), 5.0);
  EXPECT_EQ(testCam->Sample(), 5.0);
}


TEST_F(CSMCameraSetFixture, Resolution) {
  // Setup to return the ground partials we want
  // The pseudoinverse of:
  // 1 2 3
  // 4 5 6
  //
  // is
  // -17  8
  //  -2  2  *  1/18
  //  13 -4
  EXPECT_CALL(mockModel, computeGroundPartials)
      .Times(6)
      .WillRepeatedly(::testing::Return(std::vector<double>{1, 2, 3, 4, 5, 6}));

  // Use expect near here because the psuedoinverse calculation is only accurate to ~1e-10
  double expectedLineRes = sqrt(17*17 + 2*2 + 13*13)/18;
  double expectedSampRes = sqrt(8*8 + 2*2 + 4*4)/18;
  EXPECT_NEAR(testCam->LineResolution(), expectedLineRes, 1e-10);
  EXPECT_NEAR(testCam->ObliqueLineResolution(), expectedLineRes, 1e-10);
  EXPECT_NEAR(testCam->SampleResolution(), expectedSampRes, 1e-10);
  EXPECT_NEAR(testCam->ObliqueSampleResolution(), expectedSampRes, 1e-10);
  EXPECT_NEAR(testCam->DetectorResolution(), (expectedLineRes+expectedSampRes) / 2.0, 1e-10);
  EXPECT_NEAR(testCam->ObliqueDetectorResolution(), (expectedLineRes+expectedSampRes) / 2.0, 1e-10);
}


TEST_F(CSMCameraSetFixture, InstrumentBodyFixedPosition) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double position[3];
  testCam->instrumentBodyFixedPosition(position);
  EXPECT_EQ(position[0], (imageLocus.point.x) / 1000.0);
  EXPECT_EQ(position[1], (imageLocus.point.y) / 1000.0);
  EXPECT_EQ(position[2], (imageLocus.point.z) / 1000.0);
}


TEST_F(CSMCameraSetFixture, SubSpacecraftPoint) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double lat, lon;
  testCam->subSpacecraftPoint(lat, lon);
  EXPECT_EQ(lat, 0.0);
  EXPECT_EQ(lon, 0.0);
}


TEST_F(CSMCameraSetFixture, SlantDistance) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double expectedDistance = sqrt(
      pow(imageLocus.point.x - groundPt.x, 2) +
      pow(imageLocus.point.y - groundPt.y, 2) +
      pow(imageLocus.point.z - groundPt.z, 2)) / 1000.0;
  EXPECT_DOUBLE_EQ(testCam->SlantDistance(), expectedDistance);
}


TEST_F(CSMCameraSetFixture, TargetCenterDistance) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double expectedDistance = sqrt(
      pow(imageLocus.point.x, 2) +
      pow(imageLocus.point.y, 2) +
      pow(imageLocus.point.z, 2)) / 1000.0;
  EXPECT_DOUBLE_EQ(testCam->targetCenterDistance(), expectedDistance);
}


TEST_F(CSMCameraSetFixture, PhaseAngle) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefCoord(groundPt.x + 50000, groundPt.y, groundPt.z + 50000)));
  EXPECT_CALL(mockModel, getIlluminationDirection(MatchEcefCoord(groundPt)))
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefVector(0.0, 0.0, -1.0)));

  EXPECT_DOUBLE_EQ(testCam->PhaseAngle(), 45.0);
}


TEST_F(CSMCameraSetFixture, IncidenceAngle) {
  EXPECT_CALL(mockModel, getIlluminationDirection(MatchEcefCoord(groundPt)))
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefVector(0.0, 0.0, -1.0)));

  EXPECT_DOUBLE_EQ(testCam->IncidenceAngle(), 90.0);
}


TEST_F(CSMCameraSetFixture, EmissionAngle) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  EXPECT_DOUBLE_EQ(testCam->EmissionAngle(), 0.0);
}


TEST_F(CSMCameraFixture, SetTime) {
  try
  {
    testCam->setTime(iTime("2000-01-01T11:58:55.816"));
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Setting the image time is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Setting the image time is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, SubSolarPoint) {
  try
  {
    double lat, lon;
    testCam->subSolarPoint(lat ,lon);
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Sub solar point is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Sub solar point is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, PixelIfovOffsets) {
  try
  {
    testCam->PixelIfovOffsets();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Pixel Field of View is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Pixel Field of View is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, SunPosition) {
  try
  {
    double position[3];
    testCam->sunPosition(position);
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Sun position is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Sun position is not supported for CSM camera models\"";
  }

  try
  {
    testCam->sunPosition();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Sun position is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Sun position is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, InstrumentPosition) {
  try
  {
    testCam->instrumentPosition();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Instrument position is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Instrument position is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, BodyRotation) {
  try
  {
    testCam->bodyRotation();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Target body orientation is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Target body orientation is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, InstrumentRotation) {
  try
  {
    testCam->instrumentRotation();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Instrument orientation is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Instrument orientation is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, SolarLongitude) {
  try
  {
    testCam->solarLongitude();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Solar longitude is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Solar longitude is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, SolarDistance) {
  try
  {
    testCam->SolarDistance();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Solar distance is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Solar distance is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, RightAscension) {
  try
  {
    testCam->RightAscension();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Right Ascension is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Right Ascension is not supported for CSM camera models\"";
  }
}


TEST_F(CSMCameraFixture, Declination) {
  try
  {
    testCam->Declination();
  }
  catch(Isis::IException &e)
  {
    EXPECT_TRUE(e.toString().toLatin1().contains("Declination is not supported "
        "for CSM camera models")) << e.toString().toStdString();
  }
  catch(...)
  {
      FAIL() << "Expected an IException with message \""
      " Declination is not supported for CSM camera models\"";
  }
}