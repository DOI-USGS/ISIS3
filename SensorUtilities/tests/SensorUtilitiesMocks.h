#include "gmock/gmock.h"

#include "SensorUtilities.h"

using namespace SensorUtilities;

class MockShape : public Shape {
  public:
    MOCK_METHOD(Vec, intersect, (Vec sensorPos, Vec lookVec), (override));
    MOCK_METHOD(Vec, normal, (Vec groundPos), (override));
};

class MockSensor : public Sensor {
  public:
    MOCK_METHOD(Vec, lookVec, (ImagePt imagePoint), (override));
    MOCK_METHOD(Vec, j2000LookVec, (ImagePt imagePoint), (override));
    MOCK_METHOD(Vec, sensorPos, (ImagePt imagePoint), (override));
    MOCK_METHOD(double, time, (ImagePt imagePoint), (override));
};

class MockIlluminator : public Illuminator {
  public:
    MOCK_METHOD(Vec, position, (double time), (override));
};