#include "gmock/gmock.h"

#include <cmath>

#include "MathUtils.h"
#include "SensorUtilities.h"
#include "SensorUtilitiesMocks.h"

using namespace SensorUtilities;
using ::testing::Return;

TEST(PhaseAngle, Acute) {
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {100.0, 0.0, 0.0};
  Vec testIllumPos = {100.0, 100.0, 0.0};
  Vec testGroundPt = {0.0, 0.0, 0.0};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};
  Intersection testIntersect = {testGroundPt, testGroundPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));

  MockIlluminator illuminator;
  EXPECT_CALL(illuminator, position(testTime))
        .WillRepeatedly(Return(testIllumPos));

  EXPECT_DOUBLE_EQ(M_PI / 4.0, phaseAngle(testPt, &sensor, &shape, &illuminator));
}

TEST(EmissionAngle, Acute) {
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {100.0, 0.0, 0.0};
  Vec testGroundPt = {0.0, 0.0, 0.0};
  Vec testNormal = {-1.0, 0.0, 0.0};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};
  Intersection testIntersect = {testGroundPt, testNormal};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));

  EXPECT_DOUBLE_EQ(M_PI, emissionAngle(testPt, &sensor, &shape));
}

TEST(IlluminationDistance, Earth) {
  Vec testSensorLook = {1.0, 0.0, 0.0};
  Vec testSensorPos = {-100.0, 0.0, 0.0};
  Vec testIllumPos = {148180000000.0, 0.0, 0.0};
  Vec testGroundPt = {0.0, 0.0, 0.0};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};
  Intersection testIntersect = {testGroundPt, testGroundPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));

  MockIlluminator illuminator;
  EXPECT_CALL(illuminator, position(testTime))
        .WillRepeatedly(Return(testIllumPos));

  EXPECT_DOUBLE_EQ(testIllumPos.x, illuminationDistance(testPt, &sensor, &shape, &illuminator));
}

TEST(SubSpacecraftPoint, LatLon) {
  Vec testSensorLook = {1.0, 0.0, 0.0};
  Vec testSensorPos = {0.0, 0.0, 100.0};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  GroundPt2D expectedGroundPt = {M_PI / 2.0, 0.0};
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  EXPECT_EQ(expectedGroundPt, subSpacecraftPoint(testPt, &sensor));
}

TEST(SubSpacecraftPoint, LatLonRad) {
  Vec testLook = {0.0, 0.0, -100.0};
  Vec testSensorPos = {0.0, 0.0, 100.0};
  Vec testGroundPt = {0.0, 0.0, 10.0};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testLook, testLook, testSensorPos, testTime, testPt};
  Intersection testIntersect = {testGroundPt, testGroundPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testLook, true))
        .WillRepeatedly(Return(testIntersect));

  EXPECT_EQ(testGroundPt, subSpacecraftPoint(testPt, &sensor, &shape));
}

TEST(SubSolarPoint, LatLon) {
  Vec testLook = {0.0, 0.0, -100.0};
  Vec testSensorPos = {0.0, 0.0, 100.0};
  double testTime = 100.0;
  Vec testIllumPos = {0.0, 0.0, 100.0};
  ImagePt testPt = {10.0, 20.0};
  GroundPt2D expectedGroundPt = {M_PI / 2.0, 0.0};
  ObserverState testSensorState = {testLook, testLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockIlluminator illuminator;
  EXPECT_CALL(illuminator, position(testTime))
        .WillRepeatedly(Return(testIllumPos));

  EXPECT_EQ(expectedGroundPt, subSolarPoint(testPt, &sensor, &illuminator));
}

TEST(SubSolarPoint, LatLonRad) {
  double testTime = 100.0;
  Vec testLook = {0.0, 0.0, -100.0};
  Vec testSensorPos = {0.0, 0.0, 100.0};
  Vec testIllumPos = {0.0, 0.0, 100.0};
  Vec testGroundPt = {0.0, 0.0, 10.0};
  ImagePt testPt = {10.0, 20.0};
  ObserverState testSensorState = {testLook, testLook, testSensorPos, testTime, testPt};
  Intersection testIntersect = {testGroundPt, testGroundPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testIllumPos, testLook, true))
        .WillRepeatedly(Return(testIntersect));

  MockIlluminator illuminator;
  EXPECT_CALL(illuminator, position(testTime))
        .WillRepeatedly(Return(testIllumPos));

  EXPECT_EQ(testGroundPt, subSolarPoint(testPt, &sensor, &illuminator, &shape));
}

TEST(LocalRadius, Intersection) {
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {100.0, 0.0, 0.0};
  Vec testGroundPt = {10.0, 0.0, 0.0};
  double testTime = 100.0;
  ImagePt testPt = {10.0, 20.0};
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};
  Intersection testIntersect = {testGroundPt, testGroundPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));

  EXPECT_DOUBLE_EQ(10.0, localRadius(testPt, &sensor, &shape));
}

TEST(LocalRadius, Ground) {
  Vec testLook = {-1000.0, 0.0, 0.0};
  Vec testPos = {1000.0, 0.0, 0.0};
  GroundPt2D groundPt = {0.0, 0.0};
  Intersection testIntersect = {{10.0, 0.0, 0.0}, {10.0, 0.0, 0.0}};

  MockShape shape;
  EXPECT_CALL(shape, intersect(testPos, testLook, true))
        .WillRepeatedly(Return(testIntersect));

  EXPECT_DOUBLE_EQ(10.0, localRadius(groundPt, &shape, 1000.0));
}