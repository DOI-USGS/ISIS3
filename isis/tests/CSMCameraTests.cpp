#include <QString>
#include <QUuid>
#include <iostream>

#include "csm/CorrelationModel.h"
#include "csm/Ellipsoid.h"
#include "csm/RasterGM.h"

#include "Fixtures.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MockCsmPlugin.h"
#include "TestUtilities.h"
#include "StringBlob.h"
#include "FileName.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gmock/gmock.h"

using namespace Isis;

// gMock matchers for CSM structs
::testing::Matcher<const csm::ImageCoord&> MatchImageCoord(const csm::ImageCoord &expected) {
  return ::testing::AllOf(
      ::testing::Field(&csm::ImageCoord::line, ::testing::DoubleNear(expected.line, 0.0001)),
      ::testing::Field(&csm::ImageCoord::samp, ::testing::DoubleNear(expected.samp, 0.0001))
  );
}
::testing::Matcher<const csm::EcefCoord&> MatchEcefCoord(const csm::EcefCoord &expected) {
  return ::testing::AllOf(
      ::testing::Field(&csm::EcefCoord::x, ::testing::DoubleNear(expected.x, 0.0001)),
      ::testing::Field(&csm::EcefCoord::y, ::testing::DoubleNear(expected.y, 0.0001)),
      ::testing::Field(&csm::EcefCoord::z, ::testing::DoubleNear(expected.z, 0.0001))
  );
}

// Mock CSM Model class
// TODO this should also inherit from SettableEllipsoid and we need to
// mock in the MARS radii
class MockRasterGM : public csm::RasterGM {
  public:
    // csm::Model
    MOCK_METHOD(csm::Version, getVersion, (), (const, override));
    MOCK_METHOD(std::string, getModelName, (), (const, override));
    MOCK_METHOD(std::string, getPedigree, (), (const, override));
    MOCK_METHOD(std::string, getImageIdentifier, (), (const, override));
    MOCK_METHOD(void, setImageIdentifier, (const std::string&, csm::WarningList*), (override));
    MOCK_METHOD(std::string, getSensorIdentifier, (), (const, override));
    MOCK_METHOD(std::string, getPlatformIdentifier, (), (const, override));
    MOCK_METHOD(std::string, getCollectionIdentifier, (), (const, override));
    MOCK_METHOD(std::string, getTrajectoryIdentifier, (), (const, override));
    MOCK_METHOD(std::string, getSensorType, (), (const, override));
    MOCK_METHOD(std::string, getSensorMode, (), (const, override));
    MOCK_METHOD(std::string, getReferenceDateAndTime, (), (const, override));
    MOCK_METHOD(std::string, getModelState, (), (const, override));
    MOCK_METHOD(void, replaceModelState, (const std::string&), (override));
    // csm::GeometricModel methods
    MOCK_METHOD(csm::EcefCoord, getReferencePoint, (), (const, override));
    MOCK_METHOD(void, setReferencePoint, (const csm::EcefCoord&), (override));
    MOCK_METHOD(int, getNumParameters, (), (const, override));
    MOCK_METHOD(std::string, getParameterName, (int), (const, override));
    MOCK_METHOD(std::string, getParameterUnits, (int), (const, override));
    MOCK_METHOD(bool, hasShareableParameters, (), (const, override));
    MOCK_METHOD(bool, isParameterShareable, (int), (const, override));
    MOCK_METHOD(csm::SharingCriteria, getParameterSharingCriteria, (int), (const, override));
    MOCK_METHOD(double, getParameterValue, (int), (const, override));
    MOCK_METHOD(void, setParameterValue, (int, double), (override));
    MOCK_METHOD(csm::param::Type, getParameterType, (int), (const, override));
    MOCK_METHOD(void, setParameterType, (int, csm::param::Type), (override));
    MOCK_METHOD(double, getParameterCovariance, (int, int), (const, override));
    MOCK_METHOD(void, setParameterCovariance, (int, int, double), (override));
    MOCK_METHOD(int, getNumGeometricCorrectionSwitches, (), (const, override));
    MOCK_METHOD(std::string, getGeometricCorrectionName, (int), (const, override));
    MOCK_METHOD(void, setGeometricCorrectionSwitch, (int, bool, csm::param::Type), (override));
    MOCK_METHOD(bool, getGeometricCorrectionSwitch, (int), (const, override));
    MOCK_METHOD(std::vector<double>,
                getCrossCovarianceMatrix,
                (const csm::GeometricModel&, csm::param::Set, const csm::GeometricModel::GeometricModelList&),
                (const, override));
    // RasterGM methods
    MOCK_METHOD(csm::ImageCoord, groundToImage, (const csm::EcefCoord&, double, double*, csm::WarningList*), (const, override));
    MOCK_METHOD(csm::ImageCoordCovar,
                groundToImage,
                (const csm::EcefCoordCovar&, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(csm::EcefCoord,
                imageToGround,
                (const csm::ImageCoord&, double, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(csm::EcefCoordCovar,
                imageToGround,
                (const csm::ImageCoordCovar&, double, double, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(csm::EcefLocus,
                imageToProximateImagingLocus,
                (const csm::ImageCoord&, const csm::EcefCoord&, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(csm::EcefLocus,
                imageToRemoteImagingLocus,
                (const csm::ImageCoord&, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(csm::ImageCoord, getImageStart, (), (const, override));
    MOCK_METHOD(csm::ImageVector, getImageSize, (), (const, override));
    MOCK_METHOD((std::pair<csm::ImageCoord, csm::ImageCoord>), getValidImageRange, (), (const, override));
    MOCK_METHOD((std::pair<double,double>), getValidHeightRange, (), (const, override));
    MOCK_METHOD(csm::EcefVector, getIlluminationDirection, (const csm::EcefCoord&), (const, override));
    MOCK_METHOD(double, getImageTime, (const csm::ImageCoord&), (const, override));
    MOCK_METHOD(csm::EcefCoord, getSensorPosition, (const csm::ImageCoord&), (const, override));
    MOCK_METHOD(csm::EcefCoord, getSensorPosition, (double), (const, override));
    MOCK_METHOD(csm::EcefVector, getSensorVelocity, (const csm::ImageCoord&), (const, override));
    MOCK_METHOD(csm::EcefVector, getSensorVelocity, (double), (const, override));
    MOCK_METHOD(csm::RasterGM::SensorPartials,
                computeSensorPartials,
                (int, const csm::EcefCoord&, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(csm::RasterGM::SensorPartials,
                computeSensorPartials,
                (int, const csm::ImageCoord&, const csm::EcefCoord&, double, double*, csm::WarningList*),
                (const, override));
    MOCK_METHOD(std::vector<double>, computeGroundPartials, (const csm::EcefCoord&), (const, override));
    MOCK_METHOD(const csm::CorrelationModel&, getCorrelationModel, (), (const, override));
    MOCK_METHOD(std::vector<double>,
                getUnmodeledCrossCovariance,
                (const csm::ImageCoord&, const csm::ImageCoord&),
                (const, override));
};


class CSMCubeFixture : public SmallCube {
  protected:
    QString filename;
    MockRasterGM mockModel;
    Camera *testCam;

    void SetUp() override {
      SmallCube::SetUp();

      // Instrument group
      // Just need a target name
      PvlGroup instGroup("Instrument");
      instGroup += PvlKeyword("TargetName", "TestTarget");
      instGroup += PvlKeyword("InstrumentId", "TestId");
      testCube->putGroup(instGroup);

      // Kernels group
      // Just need a shapemodel specified
      PvlGroup kernGroup("Kernels");
      kernGroup += PvlKeyword("ShapeModel", "Null");
      testCube->putGroup(kernGroup);

      // CSMInfo group
      // This just has to exist, but fill it out for completeness and incase it
      // ever does matter
      PvlGroup infoGroup("CsmInfo");
      infoGroup += PvlKeyword("CSMPlatformID", "TestPlatform");
      infoGroup += PvlKeyword("CSMInstrumentId", "TestInstrument");
      infoGroup += PvlKeyword("ReferenceTime", "2000-01-01T11:58:55.816"); // J2000 epoch

      PvlKeyword paramNames("ModelParameterNames");
      paramNames += "TestNoneParam";
      paramNames += "TestFictitiousParam";
      paramNames += "TestRealParam";
      paramNames += "TestFixedParam";
      PvlKeyword paramUnits("ModelParameterUnits");
      paramUnits += "unitless";
      paramUnits += "m";
      paramUnits += "rad";
      paramUnits += "lines/sec";
      PvlKeyword paramTypes("ModelParameterTypes");
      paramTypes += "NONE";
      paramTypes += "FICTITIOUS";
      paramTypes += "REAL";
      paramTypes += "FIXED";

      infoGroup += paramNames;
      infoGroup += paramUnits;
      infoGroup += paramTypes;

      testCube->putGroup(infoGroup);

      // Register the mock with our plugin
      std::string mockModelName = QUuid().toString().toStdString();
      MockCsmPlugin loadablePlugin;
      loadablePlugin.registerModel(mockModelName, &mockModel);

      // CSMState BLOB
      StringBlob csmStateBlob(mockModelName, "CSMState");
      csmStateBlob.Label() += PvlKeyword("ModelName", QString::fromStdString(mockModelName));
      csmStateBlob.Label() += PvlKeyword("PluginName", QString::fromStdString(loadablePlugin.getPluginName()));
      testCube->write(csmStateBlob);
      filename = testCube->fileName();
      testCube->close();
      testCube->open(filename, "rw");
    }
};


class CSMCameraFixture : public CSMCubeFixture {
  protected:
    Camera *testCam;

    void SetUp() override {
      CSMCubeFixture::SetUp();

      // Account for calls that happen while making a CSMCamera
      EXPECT_CALL(mockModel, getSensorIdentifier())
          .Times(2)
          .WillRepeatedly(::testing::Return("MockSensorID"));
      EXPECT_CALL(mockModel, getPlatformIdentifier())
          .Times(2)
          .WillRepeatedly(::testing::Return("MockPlatformID"));
      EXPECT_CALL(mockModel, getReferenceDateAndTime())
          .Times(1)
          .WillRepeatedly(::testing::Return("20000101T115855.816"));

      testCam = testCube->camera();
    }
};


class CSMSetCameraFixture : public CSMCameraFixture {
  protected:
    csm::Ellipsoid wgs84;
    csm::ImageCoord imagePt;
    csm::EcefCoord groundPt;
    csm::EcefLocus imageLocus;

    void SetUp() override {
      CSMCameraFixture::SetUp();

      imagePt = csm::ImageCoord(4.5, 4.5);
      groundPt = csm::EcefCoord(wgs84.getSemiMajorRadius(), 0, 0);
      imageLocus = csm::EcefLocus(wgs84.getSemiMajorRadius() + 50000, 0, 0, -1, 0, 0);

      // Setup the mock for setImage and ensure it succeeds
      EXPECT_CALL(mockModel, imageToRemoteImagingLocus(MatchImageCoord(imagePt), ::testing::_, ::testing::_, ::testing::_))
          .Times(1)
          .WillOnce(::testing::Return(imageLocus));
      EXPECT_CALL(mockModel, getImageTime)
          .Times(1)
          .WillOnce(::testing::Return(10.0));

      ASSERT_TRUE(testCam->SetImage(5, 5)); // Assert here so that the test code doesn't run if the camera isn't set
    }
};


class CSMDemCameraFixture : public CSMCubeFixture {
  protected:
    Camera *testCam;
    double demRadius;

    void SetUp() override {
      CSMCubeFixture::SetUp();

      // Record the demRadius at 0 lat, 0 lon
      demRadius = 3394200.43980104;

      // Update the shapemodel on the cube
      PvlGroup &kernGroup = testCube->group("Kernels");
      kernGroup.addKeyword(PvlKeyword("ShapeModel", "data/CSMCamera/mola_compressed_prep.cub"), Pvl::Replace);

      // Close and re-open the cube, then save off the new camera
      testCube->close();
      testCube->open(filename, "rw");

      // Account for calls that happen while making a CSMCamera
      EXPECT_CALL(mockModel, getSensorIdentifier())
          .Times(2)
          .WillRepeatedly(::testing::Return("MockSensorID"));
      EXPECT_CALL(mockModel, getPlatformIdentifier())
          .Times(2)
          .WillRepeatedly(::testing::Return("MockPlatformID"));
      EXPECT_CALL(mockModel, getReferenceDateAndTime())
          .Times(1)
          .WillRepeatedly(::testing::Return("20000101T115855.816"));

      testCam = testCube->camera();
    }
};


TEST(CSMCameraTest, MockTest) {
  MockRasterGM mockModel;
  EXPECT_CALL(mockModel, getVersion())
      .Times(1)
      .WillOnce(::testing::Return(csm::Version(1, 2, 3)));
  csm::Version mockVersion = mockModel.getVersion();
  EXPECT_EQ(mockVersion.major(), 1);
  EXPECT_EQ(mockVersion.minor(), 2);
  EXPECT_EQ(mockVersion.revision(), 3);
}


TEST(CSMCameraTest, LoadMockTest) {
  MockRasterGM mockModel;
  EXPECT_CALL(mockModel, getVersion())
      .Times(1)
      .WillOnce(::testing::Return(csm::Version(1, 2, 3)));

  // Use a universally unique identifier for thread safety
  std::string mockModelName = QUuid().toString().toStdString();
  MockCsmPlugin loadablePlugin;
  loadablePlugin.registerModel(mockModelName, &mockModel);

  csm::Model *returnedModel = csm::Plugin::findPlugin(MockCsmPlugin::PLUGIN_NAME)->constructModelFromState(mockModelName);
  csm::Version mockVersion = returnedModel->getVersion();
  EXPECT_EQ(mockVersion.major(), 1);
  EXPECT_EQ(mockVersion.minor(), 2);
  EXPECT_EQ(mockVersion.revision(), 3);
}

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


TEST_F(CSMDemCameraFixture, SetImage) {
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


TEST_F(CSMDemCameraFixture, SetGround) {
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


TEST_F(CSMSetCameraFixture, Resolution) {
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


TEST_F(CSMSetCameraFixture, InstrumentBodyFixedPosition) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double position[3];
  testCam->instrumentBodyFixedPosition(position);
  EXPECT_EQ(position[0], (imageLocus.point.x) / 1000.0);
  EXPECT_EQ(position[1], (imageLocus.point.y) / 1000.0);
  EXPECT_EQ(position[2], (imageLocus.point.z) / 1000.0);
}


TEST_F(CSMSetCameraFixture, SubSpacecraftPoint) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double lat, lon;
  testCam->subSpacecraftPoint(lat, lon);
  EXPECT_EQ(lat, 0.0);
  EXPECT_EQ(lon, 0.0);
}


TEST_F(CSMSetCameraFixture, SlantDistance) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double expectedDistance = sqrt(
      pow(imageLocus.point.x - groundPt.x, 2) +
      pow(imageLocus.point.y - groundPt.y, 2) +
      pow(imageLocus.point.z - groundPt.z, 2)) / 1000.0;
  EXPECT_DOUBLE_EQ(testCam->SlantDistance(), expectedDistance);
}


TEST_F(CSMSetCameraFixture, TargetCenterDistance) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(imageLocus.point));

  double expectedDistance = sqrt(
      pow(imageLocus.point.x, 2) +
      pow(imageLocus.point.y, 2) +
      pow(imageLocus.point.z, 2)) / 1000.0;
  EXPECT_DOUBLE_EQ(testCam->targetCenterDistance(), expectedDistance);
}


TEST_F(CSMSetCameraFixture, PhaseAngle) {
  EXPECT_CALL(mockModel, getSensorPosition(MatchImageCoord(imagePt)))
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefCoord(groundPt.x + 50000, groundPt.y, groundPt.z + 50000)));
  EXPECT_CALL(mockModel, getIlluminationDirection(MatchEcefCoord(groundPt)))
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefVector(0.0, 0.0, 1.0)));

  EXPECT_DOUBLE_EQ(testCam->PhaseAngle(), 45.0);
}


TEST_F(CSMSetCameraFixture, IncidenceAngle) {
  EXPECT_CALL(mockModel, getIlluminationDirection(MatchEcefCoord(groundPt)))
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefVector(0.0, 0.0, 1.0)));

  EXPECT_DOUBLE_EQ(testCam->IncidenceAngle(), 90.0);
}


TEST_F(CSMSetCameraFixture, EmissionAngle) {
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
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
      FAIL() << "Expected an IExcpetion with message \""
      " Declination is not supported for CSM camera models\"";
  }
}