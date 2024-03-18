#include "gmock/gmock.h"

#include "SensorUtilities.h"

using namespace SensorUtilities;

class MockShape : public Shape {
  public:
    MOCK_METHOD(Intersection, intersect, (const Vec &sensorPos, const Vec &lookVec, bool computeLocalNormal), (override));
};

class MockSensor : public Sensor {
  public:
    MOCK_METHOD(ObserverState, getState, (const ImagePt &imagePoint), (override));
    MOCK_METHOD(ObserverState, getState, (const GroundPt3D &groundPt), (override));
};

class MockIlluminator : public Illuminator {
  public:
    MOCK_METHOD(Vec, position, (double time), (override));
    MOCK_METHOD(Vec, velocity, (double time), (override));
};

class MockBody : public Body {
  public:
    MOCK_METHOD(std::vector<double>, rotation, (double time), (override));
    MOCK_METHOD(Vec, fixedVector, (Vec pos), (override));
};