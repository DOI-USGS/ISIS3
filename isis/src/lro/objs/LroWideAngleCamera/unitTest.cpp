#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main(void) {
  Isis::Preference::Preferences(true);

  cout << "Unit Test for LroWideAngleCamera..." << endl;

  try {

    // Support different camera model versions thusly...
    Isis::Pvl p("$lro/testData/wacCameraTest.cub");
    int cmVersion = Isis::CameraFactory::CameraVersion(p);

    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat, knownLon;

    if (cmVersion == 1) {
     knownLat = -70.69638475050628;
     knownLon =  244.3314992195277;
     p.Read("$lro/testData/wacCameraTest.cub.cv1");
    }
    else {
      // Version 2 difference caused by new CK and comprehensive IK kernel support
      knownLat = -70.69650506131094;
      knownLon = 244.30134137129;
    }


    Isis::Camera *cam = Isis::CameraFactory::Create(p);
    cout << setprecision(9);

    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 1.0, 15.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), 15.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 1.0, 56);

    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), 56);

    double samp = cam->Samples() / 2;
    double line = cam->Lines() / 2;
    cout << "For center pixel position ..." << endl;

    if(!cam->SetImage(samp, line)) {
      std::cout << "ERROR" << std::endl;
      return 0;
    }

#if 0
    cout << "Latitude  = " << setprecision(16) << cam->UniversalLatitude() << endl;
    cout << "Longitude = " << setprecision(16) << cam->UniversalLongitude() << endl;
#endif

    if(abs(cam->UniversalLatitude() - knownLat) < 1E-10) {
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
    }

    if(abs(cam->UniversalLongitude() - knownLon) < 1E-10) {
      cout << "Longitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon << endl;
    }
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}

void TestLineSamp(Isis::Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if(success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if(fabs(deltaSamp) < 0.01) deltaSamp = 0;
    if(fabs(deltaLine) < 0.01) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

