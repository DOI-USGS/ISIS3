using namespace std;

#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for HiriseCamera..." << endl;
  /** HIRISE: The line,samp to lat,lon to line,samp tolerance was increased for this
   *  camera model test.
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    //    double knownLat = -1.111697540639317;
    //    double knownLon = 203.272499681125;
    double knownLat = -1.111697574811016;
    double knownLon = 203.2724994115664;

    Isis::Pvl p("$mro/testData/PSP_001446_1790_BG12_0.cub");
    Isis::Camera *cam = Isis::CameraFactory::Create(p);
    cout << setprecision(9);
   
    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 1.0, 1.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), 1.0);

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

    if(abs(cam->UniversalLongitude() - knownLon) < 1E-10) {
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
    if (fabs(deltaSamp) < 0.08) deltaSamp = 0;
    if (fabs(deltaLine) < 0.08) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

