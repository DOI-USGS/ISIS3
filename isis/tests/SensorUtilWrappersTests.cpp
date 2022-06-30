#include "gmock/gmock.h"

#include "Mocks.h"

#include "CsmSensor.h"
#include "Displacement.h"
#include "IsisIlluminator.h"
#include "IsisSensor.h"
#include "IsisShape.h"
#include "SurfacePoint.h"

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