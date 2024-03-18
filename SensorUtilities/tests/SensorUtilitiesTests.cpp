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

TEST(SlantDistance, Earth) {
  Vec testSensorLook = {1.0, 0.0, 0.0};
  Vec testSensorPos = {-100.0, 0.0, 0.0};
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

  EXPECT_DOUBLE_EQ(100.0, slantDistance(testPt, &sensor, &shape));
}


TEST(TargetCenterDistance, default) {
  Vec testSensorLook = {1.0, 0.0, 0.0};
  Vec testSensorPos = {-100.0, 0.0, 0.0};
  ImagePt testPt = {0.0, 0.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};
  Vec testBfVector = {100, 0, 0};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockBody body;
  EXPECT_CALL(body, fixedVector(testSensorPos))
        .WillRepeatedly(Return(testBfVector));

  EXPECT_DOUBLE_EQ(100, targetCenterDistance(testPt, &sensor, &body));
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

TEST(RightAscensionDeclination, RaDec) {
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {100.0, 0.0, 0.0};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  RaDec results = rightAscensionDeclination(testPt, &sensor);

  EXPECT_DOUBLE_EQ(180.0, results.ra);
  EXPECT_DOUBLE_EQ(0.0, results.dec);
}

TEST(LocalSolarTime, Lst) {
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {100.0, 0.0, 0.0};
  Vec testIllumPos = {100.0, 100.0, 0.0};
  Intersection testIntersect = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));

  MockIlluminator illuminator;
  EXPECT_CALL(illuminator, position(testTime))
        .WillRepeatedly(Return(testIllumPos));

  double result = localSolarTime(testPt, &sensor, &shape, &illuminator);

  EXPECT_DOUBLE_EQ(9.0, result);
}

TEST(SampleResolution, Double) {
  double focalLength = 175.01;
  double pixelPitch = 0.007;
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {-182.681, -29.6026, 92.2182};
  Intersection testIntersect = {{-11.7277, -0.383773, 4.4394}, {0.0, 0.0, 0.0}};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));


  double result = sampleResolution(testPt, &sensor, &shape, focalLength, pixelPitch);

  EXPECT_NEAR(7.7748, result, 0.0001);
}


TEST(LineResolution, Double) {
  double focalLength = 175.01;
  double pixelPitch = 0.007;
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {-182.681, -29.6026, 92.2182};
  Intersection testIntersect = {{-11.7277, -0.383773, 4.4394}, {0.0, 0.0, 0.0}};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));


  double result = lineResolution(testPt, &sensor, &shape, focalLength, pixelPitch);

  EXPECT_NEAR(7.7748, result, 0.0001);
}


TEST(PixelResolution, Double) {
  double focalLength = 175.01;
  double pixelPitch = 0.007;
  Vec testSensorLook = {-1.0, 0.0, 0.0};
  Vec testSensorPos = {-182.681, -29.6026, 92.2182};
  Intersection testIntersect = {{-11.7277, -0.383773, 4.4394}, {0.0, 0.0, 0.0}};
  ImagePt testPt = {10.0, 20.0};
  double testTime = 100.0;
  ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

  MockSensor sensor;
  EXPECT_CALL(sensor, getState(testPt))
        .WillRepeatedly(Return(testSensorState));

  MockShape shape;
  EXPECT_CALL(shape, intersect(testSensorPos, testSensorLook, true))
        .WillRepeatedly(Return(testIntersect));


  double result = pixelResolution(testPt, &sensor, &shape, focalLength, pixelPitch);

  EXPECT_NEAR(7.7748, result, 0.0001);
}


TEST(SolarLongitude, Vec) {
      Vec testSensorLook = {-1.0, 0.0, 0.0};
      Vec testSensorPos = {100.0, 0.0, 0.0};
      Vec testIllumPos = {-9.5854e+07, 1.69532e+08, 8.03473e+07};
      Vec testIllumVel = {-20.7053, -11.7278, -5.8646};
      std::vector<double> testBodyRot = { 0.011128, -0.895746, -0.444427, 
                                          0.889656, 0.211773, -0.404554, 
                                          0.456496, -0.390886, 0.799262};
      ImagePt testPt = {10.0, 20.0};
      double testTime = 100.0;
      ObserverState testSensorState = {testSensorLook, testSensorLook, testSensorPos, testTime, testPt};

      MockSensor sensor;
      EXPECT_CALL(sensor, getState(testPt))
            .WillRepeatedly(Return(testSensorState));

      MockIlluminator illuminator;
      EXPECT_CALL(illuminator, position(testTime))
            .WillRepeatedly(Return(testIllumPos));
      EXPECT_CALL(illuminator, velocity(testTime))
            .WillRepeatedly(Return(testIllumVel));

      MockBody body;
      EXPECT_CALL(body, rotation(testTime))
            .WillRepeatedly(Return(testBodyRot));

      double result = solarLongitude(testPt, &sensor, &illuminator, &body);

      EXPECT_NEAR(207.8443, result, 0.0001);
}

