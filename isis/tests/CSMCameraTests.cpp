#include <QString>
#include <QUuid>
#include <iostream>

#include "csm/CorrelationModel.h"
#include "csm/Ellipsoid.h"
#include "csm/RasterGM.h"

#include "AlternativeTestCsmModel.h"
#include "TestCsmPlugin.h"
#include "MockCsmPlugin.h"
#include "Fixtures.h"
#include "TestUtilities.h"
#include "TestCsmModel.h"
#include "StringBlob.h"
#include "FileName.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gmock/gmock.h"

using namespace Isis;

// Mock CSM Model class
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

class CSMCameraFixture : public SmallCube {
  protected:
    QString filename;
    MockRasterGM mockModel;

    void SetUp() override {
      SmallCube::SetUp();

      // Instrument group
      // Just need a target name
      PvlGroup instGroup("Instrument");
      instGroup += PvlKeyword("TargetName", "TestTarget");
      instGroup += PvlKeyword("InstrumentId", "TestId");
      testCube->putGroup(instGroup);

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

      // Account for calls that happen while making a CSMCamera
      EXPECT_CALL(mockModel, getSensorIdentifier())
          .Times(2)
          .WillRepeatedly(::testing::Return("MockSensorID"));
      EXPECT_CALL(mockModel, getPlatformIdentifier())
          .Times(2)
          .WillRepeatedly(::testing::Return("MockPlatformID"));

      // CSMState BLOB
      StringBlob csmStateBlob(mockModelName, "CSMState");
      csmStateBlob.Label() += PvlKeyword("ModelName", QString::fromStdString(mockModelName));
      csmStateBlob.Label() += PvlKeyword("PluginName", QString::fromStdString(loadablePlugin.getPluginName()));
      testCube->write(csmStateBlob);
      filename = testCube->fileName();
      testCube->close();
      testCube->open(filename);
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
  Camera *testCam = testCube->camera();
  csm::Ellipsoid wgs84;
  // EXPECT_CALL(mockModel, imageToRemoteImagingLocus(csm::ImageCoord(4.5, 4.5), _, _, _))
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus)
      .Times(1)
      .WillOnce(::testing::Return(csm::EcefLocus(wgs84.getSemiMajorRadius() + 50000, 0, 0, -1, 0, 0))); // looking straight down X-Axis

  testCam->SetImage(5, 5);
  EXPECT_EQ(testCam->UniversalLatitude(), 0.0);
  EXPECT_EQ(testCam->UniversalLongitude(), 0.0);
}