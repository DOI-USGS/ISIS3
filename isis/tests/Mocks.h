#ifndef Mocks_h
#define Mocks_h

#include "gmock/gmock.h"

#include "Cube.h"
#include "Camera.h"
#include "Interpolator.h"
#include "Process.h"
#include "ProcessRubberSheet.h"
#include "TProjection.h"
#include "Transform.h"

using namespace Isis;

class MockCube : public Cube {
  public:
    MOCK_CONST_METHOD0(bandCount, int());
    MOCK_CONST_METHOD0(fileName, QString());
    MOCK_CONST_METHOD1(physicalBand, int(const int &virtualBand));
    MOCK_METHOD4(histogram, Histogram*(
          const int &band, const double &validMin,
          const double &validMax,
          QString msg));
};

class MockCamera : public Camera {
  public:
    MockCamera(Cube &cube): Camera(cube) {}
    MOCK_METHOD(bool, SetImage, (const double sample, const double line), (override));
    MOCK_METHOD(double, Line, (), (const, override));
    MOCK_METHOD(double, Sample, (), (const, override));
    MOCK_METHOD(double, UniversalLatitude, (), (const override));
    MOCK_METHOD(double, UniversalLongitude, (), (const override));
    MOCK_METHOD(bool, SetUniversalGround, (const double latitude, const double longitude), (override));
    MOCK_METHOD(bool, SetUniversalGround, (const double latitude, const double longitude,
                                           const double radius), (override));
    MOCK_METHOD(CameraType, GetCameraType, (), (const, override));
    MOCK_METHOD(int, CkFrameId, (), (const override));
    MOCK_METHOD(int, CkReferenceId, (), (const override));
    MOCK_METHOD(int, SpkReferenceId, (), (const override));
};

class MockTProjection : public TProjection {
  public:
    MockTProjection(Pvl &label): TProjection(label) {}
    MOCK_METHOD(bool, SetWorld, (const double x, const double y), (override));
    MOCK_METHOD(bool, HasGroundRange, (), (const override));
    MOCK_METHOD(double, UniversalLatitude, (), (override));
    MOCK_METHOD(double, UniversalLongitude, (), (override));
    MOCK_METHOD(double, Latitude, (), (const override));
    MOCK_METHOD(double, MinimumLatitude, (), (const override));
    MOCK_METHOD(double, MaximumLatitude, (), (const override));
    MOCK_METHOD(double, Longitude, (), (const override));
    MOCK_METHOD(double, MinimumLongitude, (), (const override));
    MOCK_METHOD(double, MaximumLongitude, (), (const override));
    MOCK_METHOD(double, WorldX, (), (const override));
    MOCK_METHOD(double, WorldY, (), (const override));
    MOCK_METHOD(bool, SetUniversalGround, (const double lat, const double lon), (override));
    // MOCK_METHOD(CameraType, GetCameraType, (), (const, override));
    MOCK_METHOD(QString, Name, (), (const override));
    MOCK_METHOD(QString, Version, (), (const override));
    // MOCK_METHOD(int, SpkReferenceId, (), (const override));
};

class MockProcessRubberSheet : public ProcessRubberSheet {
public:
  MOCK_METHOD(void, StartProcess, (Transform &trans, Interpolator &interp), (override));
  MOCK_METHOD(Cube*, SetOutputCube, (const QString &fname,
                                     const Isis::CubeAttributeOutput &att,
                                     const int ns, const int nl,
                                     const int nb), (override));
  MOCK_METHOD(void, SetInputCube, (Cube *inCube), (override));
  MOCK_METHOD(void, processPatchTransform, (Transform &trans, Interpolator &interp), (override));
  MOCK_METHOD(void, setPatchParameters, (int startSample, int startLine, int samples,
                                         int lines, int sampleIncrement, int lineIncrement), (override));
  MOCK_METHOD(void, forceTile, (double Samp, double Line));
  MOCK_METHOD(void, SetTiling, (long long start, long long end));
  MOCK_METHOD(void, EndProcess, (), (override));
  MOCK_METHOD(void, BandChange, (void (*funct)(const int band)), (override));
};


#endif
