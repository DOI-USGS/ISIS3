#include "gmock/gmock.h"

#include "Mocks.h"

#include "csm.h"

#include "CsmSensor.h"
#include "Displacement.h"
#include "IsisIlluminator.h"
#include "IsisSensor.h"
#include "IsisShape.h"
#include "SurfacePoint.h"

// Custom csm equality operators for mock matching
namespace csm {
  bool operator==(const ImageCoord &lhs, const ImageCoord &rhs) {
    return lhs.line == rhs.line && lhs.samp == rhs.samp;
  }


  bool operator==(const EcefCoord &lhs, const EcefCoord &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }
}

TEST(IsisIlluminator, PositionNewTime) {
  double testTime = 10.0;
  double oldTime = testTime - 1;
  std::vector<double> testPosKm = {-1.0, 1.0, 2.0};
  SensorUtilities::Vec testPosM = {-1000.0, 1000.0, 2000.0};

  MockSpicePosition mockSpice(0, 1);
  EXPECT_CALL(mockSpice, EphemerisTime)
        .WillRepeatedly(::testing::Return(oldTime));

  EXPECT_CALL(mockSpice, SetEphemerisTime(testTime))
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  EXPECT_CALL(mockSpice, Coordinate)
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  EXPECT_CALL(mockSpice, SetEphemerisTime(oldTime))
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  IsisIlluminator testIlluminator(&mockSpice);

  EXPECT_EQ(testIlluminator.position(testTime), testPosM);
}

TEST(IsisIlluminator, PositionOldTime) {
  double testTime = 10.0;
  std::vector<double> testPosKm = {-1.0, 1.0, 2.0};
  SensorUtilities::Vec testPosM = {-1000.0, 1000.0, 2000.0};

  MockSpicePosition mockSpice(0, 1);
  EXPECT_CALL(mockSpice, EphemerisTime)
        .WillRepeatedly(::testing::Return(testTime));

  EXPECT_CALL(mockSpice, Coordinate)
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  IsisIlluminator testIlluminator(&mockSpice);

  EXPECT_EQ(testIlluminator.position(testTime), testPosM);
}

TEST(IsisShape, IntersectStandardNormal) {
  SensorUtilities::Vec sensorPos = {1000.0, 0.0, 0.0};
  SensorUtilities::Vec lookVec = {-1.0, 0.0, 0.0};
  SensorUtilities::Vec groundPt = {100.0, 0.0, 0.0};
  SensorUtilities::Vec testNormal = {1.0, 0.0, 0.0};
  SurfacePoint testPoint(
        Displacement(groundPt.x, Displacement::Meters),
        Displacement(groundPt.y, Displacement::Meters),
        Displacement(groundPt.z, Displacement::Meters));

  MockShapeModel mockShape;
  EXPECT_CALL(mockShape, intersectSurface(std::vector<double>(sensorPos), std::vector<double>(lookVec)));
  EXPECT_CALL(mockShape, surfaceIntersection())
        .WillRepeatedly(::testing::Return(&testPoint));
  EXPECT_CALL(mockShape, calculateSurfaceNormal);
  EXPECT_CALL(mockShape, normal())
        .WillRepeatedly(::testing::Return(testNormal));

  IsisShape testShape(&mockShape);

  SensorUtilities::Intersection intersection = testShape.intersect(sensorPos, lookVec, false);

  EXPECT_EQ(intersection.groundPt, groundPt);
  EXPECT_EQ(intersection.normal, testNormal);
}

TEST(IsisShape, IntersectDEM) {
  SensorUtilities::Vec sensorPos = {1000.0, 0.0, 0.0};
  SensorUtilities::Vec lookVec = {-1.0, 0.0, 0.0};
  SensorUtilities::Vec groundPt = {100.0, 0.0, 0.0};
  SensorUtilities::Vec testNormal = {1.0, 0.0, 0.0};
  SurfacePoint testPoint(
        Displacement(groundPt.x, Displacement::Meters),
        Displacement(groundPt.y, Displacement::Meters),
        Displacement(groundPt.z, Displacement::Meters));

  MockShapeModel mockShape;
  EXPECT_CALL(mockShape, intersectSurface(std::vector<double>(sensorPos), std::vector<double>(lookVec)));
  EXPECT_CALL(mockShape, surfaceIntersection())
        .WillRepeatedly(::testing::Return(&testPoint));
  EXPECT_CALL(mockShape, isDEM())
        .WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(mockShape, calculateSurfaceNormal);
  EXPECT_CALL(mockShape, normal())
        .WillRepeatedly(::testing::Return(testNormal));

  IsisShape testShape(&mockShape);

  SensorUtilities::Intersection intersection = testShape.intersect(sensorPos, lookVec, true);

  EXPECT_EQ(intersection.groundPt, groundPt);
  EXPECT_EQ(intersection.normal, testNormal);
}

TEST(IsisShape, IntersectLocalNormal) {
  SensorUtilities::Vec sensorPos = {1000.0, 0.0, 0.0};
  SensorUtilities::Vec lookVec = {-1.0, 0.0, 0.0};
  SensorUtilities::Vec groundPt = {100.0, 0.0, 0.0};
  SensorUtilities::Vec testNormal = {1.0, 0.0, 0.0};
  SurfacePoint testPoint(
        Displacement(groundPt.x, Displacement::Meters),
        Displacement(groundPt.y, Displacement::Meters),
        Displacement(groundPt.z, Displacement::Meters));

  MockShapeModel mockShape;
  EXPECT_CALL(mockShape, intersectSurface(std::vector<double>(sensorPos), std::vector<double>(lookVec)));
  EXPECT_CALL(mockShape, surfaceIntersection())
        .WillRepeatedly(::testing::Return(&testPoint));
  EXPECT_CALL(mockShape, isDEM())
        .WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(mockShape, calculateLocalNormal);
  EXPECT_CALL(mockShape, normal())
        .WillRepeatedly(::testing::Return(testNormal));

  IsisShape testShape(&mockShape);

  SensorUtilities::Intersection intersection = testShape.intersect(sensorPos, lookVec, true);

  EXPECT_EQ(intersection.groundPt, groundPt);
  EXPECT_EQ(intersection.normal, testNormal);
}

TEST(CsmSensor, GetStateImage) {
  SensorUtilities::Vec sensorPos = {1000.0, 0.0, 0.0};
  SensorUtilities::Vec lookVec = {-1.0, 0.0, 0.0};
  SensorUtilities::ImagePt testImagePt = {10, 20};
  double testTime = 0.5;

  // Test orientation goes from no rotation at 0 to 180 degrees around x-axis at 1
  ale::Orientations testOrientations({{0, 0, 0, 0}, {0, 1, 0, 0}}, {0, 1});

  ale::Vec3d j2000AleLookVec = testOrientations.rotateVectorAt(
        testTime, ale::Vec3d(lookVec.x ,lookVec.y, lookVec.z));
  SensorUtilities::Vec j2000LookVec(j2000AleLookVec.x, j2000AleLookVec.y, j2000AleLookVec.z);

  csm::ImageCoord testCsmCoord(testImagePt.line, testImagePt.sample);
  csm::EcefLocus testCsmLocus(sensorPos.x, sensorPos.y, sensorPos.z, lookVec.x, lookVec.y, lookVec.z);

  MockRasterGM mockModel;
  EXPECT_CALL(mockModel, getImageTime(testCsmCoord))
        .WillRepeatedly(::testing::Return(testTime));
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus(testCsmCoord, ::testing::_, ::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(testCsmLocus));

  CsmSensor testSensor(&mockModel, &testOrientations);

  SensorUtilities::ObserverState obsState = testSensor.getState(testImagePt);

  EXPECT_EQ(obsState.lookVec, lookVec);
  EXPECT_EQ(obsState.j2000LookVec, j2000LookVec);
  EXPECT_EQ(obsState.sensorPos, sensorPos);
  EXPECT_EQ(obsState.time, testTime);
  EXPECT_EQ(obsState.imagePoint, testImagePt);
}

TEST(CsmSensor, GetStateGround) {
  SensorUtilities::GroundPt3D testGroundPt = {0.0, 0.0, 100.0};
  SensorUtilities::Vec sensorPos = {1000.0, 0.0, 0.0};
  SensorUtilities::Vec lookVec = {-1.0, 0.0, 0.0};
  SensorUtilities::ImagePt testImagePt = {10, 20};
  double testTime = 0.5;

  // Test orientation goes from no rotation at 0 to 180 degrees around x-axis at 1
  ale::Orientations testOrientations({{0, 0, 0, 0}, {0, 1, 0, 0}}, {0, 1});

  ale::Vec3d j2000AleLookVec = testOrientations.rotateVectorAt(
        testTime, ale::Vec3d(lookVec.x ,lookVec.y, lookVec.z));
  SensorUtilities::Vec j2000LookVec(j2000AleLookVec.x, j2000AleLookVec.y, j2000AleLookVec.z);

  SensorUtilities::Vec testGroundVec = SensorUtilities::sphericalToRect(testGroundPt);
  csm::EcefCoord testCsmGroundPt = {testGroundVec.x, testGroundVec.y, testGroundVec.z};
  csm::ImageCoord testCsmCoord(testImagePt.line, testImagePt.sample);
  csm::EcefLocus testCsmLocus(sensorPos.x, sensorPos.y, sensorPos.z, lookVec.x, lookVec.y, lookVec.z);

  MockRasterGM mockModel;
  EXPECT_CALL(mockModel, groundToImage(testCsmGroundPt, ::testing::_, ::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(testCsmCoord));
  EXPECT_CALL(mockModel, getImageTime(testCsmCoord))
        .WillRepeatedly(::testing::Return(testTime));
  EXPECT_CALL(mockModel, imageToRemoteImagingLocus(testCsmCoord, ::testing::_, ::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(testCsmLocus));

  CsmSensor testSensor(&mockModel, &testOrientations);

  SensorUtilities::ObserverState obsState = testSensor.getState(testGroundPt);

  EXPECT_EQ(obsState.lookVec, lookVec);
  EXPECT_EQ(obsState.j2000LookVec, j2000LookVec);
  EXPECT_EQ(obsState.sensorPos, sensorPos);
  EXPECT_EQ(obsState.time, testTime);
  EXPECT_EQ(obsState.imagePoint, testImagePt);
}