#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for ThemisVisCamera..." << endl;
  /**
   * unit test changed for themis vis camera
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    //    double knownLat = 48.53342079272975;
    //    double knownLon = 332.0434164961534;
    double knownLat = 48.53342078845403;
    double knownLon = 332.0434164973285;

    Isis::Pvl p("$odyssey/testData/V14093008RDR.even.cub");
    Isis::Camera *cam = Isis::CameraFactory::Create(p);
    cout << setprecision(9);
   
    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 1.0, 193.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), 193.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 1.0, cam->Lines());

    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), cam->Lines());

    double samp = cam->Samples() / 2;
    double line = cam->Lines() / 2;
    cout << "For center pixel position ..." << endl;

    if(!cam->SetImage(samp,line)) {
      std::cout << "ERROR" << std::endl;
      return 0;
    }

    if(abs(cam->UniversalLatitude() - knownLat) < 1E-10) {
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
    }

    if(abs(cam->UniversalLongitude() - knownLon) < 2E-10) {
      cout << "Longitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon << endl;
    }
  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

void TestLineSamp(Isis::Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp,line);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if(success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if (fabs(deltaSamp) < 1.01) deltaSamp = 0;
    if (fabs(deltaLine) < 0.005) deltaLine = 0;

    if(fabs(fabs(deltaSamp) - 5.04) < .001) deltaSamp = 0;
    if(fabs(fabs(deltaSamp) - 3.919) < .001) deltaSamp = 0;
    if(fabs(fabs(deltaLine) - 19.1) < .01) deltaLine = 0;

    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

