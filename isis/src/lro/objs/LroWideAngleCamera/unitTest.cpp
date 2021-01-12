/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "LroWideAngleCamera.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

// 2013-12-19 We will have new CK kernels with temperature dependence coming
// that will affect the lat/lon values here.  Also must get a new FK, IK and
// IAK.  Don;t forget to move the new test cube to $ISISROOT/lro/testData!!!!

void TestLineSamp(Camera *cam, double samp, double line);

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for LroWideAngleCamera..." << endl;

  try {

    // Support different camera model versions thusly...
    Cube cube;
    cube.open("$lro/testData/wacCameraTest.cub");
    int cmVersion = CameraFactory::CameraVersion(cube);

    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat, knownLon;

    // Camera Version 1's test is provided here for easy testing with an old version of the camera,
    //   this is generally not executed
    if (cmVersion == 1) {
     knownLat = -70.69638475050628;
     knownLon =  244.3314992195277;
     cube.open("$lro/testData/wacCameraTest.cub.cv1");
    }
    else {
      // Version 2 or higher difference caused by new CK and comprehensive IK
      //   kernel support and no longer using a DEM
      knownLat = -70.7067960917672735;
      knownLon = 244.3369098738304217;
    }


    Camera *cam = CameraFactory::Create(cube);
    cout << "FileName: " << FileName(cube.fileName()).name() << endl;
    cout << "CK Frame: " << cam->instrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(9);
    
    // Test kernel IDs
    cout << "Kernel IDs: " << endl;
    cout << "CK Frame ID = " << cam->CkFrameId() << endl;
    cout << "CK Reference ID = " << cam->CkReferenceId() << endl;
    cout << "SPK Target ID = " << cam->SpkTargetId() << endl;
    cout << "SPK Reference ID = " << cam->SpkReferenceId() << endl << endl;
    
    // Test name methods
    cout << "Spacecraft Name Long: " << cam->spacecraftNameLong() << endl;
    cout << "Spacecraft Name Short: " << cam->spacecraftNameShort() << endl;
    cout << "Instrument Name Long: " << cam->instrumentNameLong() << endl;
    cout << "Instrument Name Short: " << cam->instrumentNameShort() << endl << endl;

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
      cout << "ERROR" << endl;
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
    
    Cube c2("$lro/testData/out.uv.even.cub", "r");
    Camera *cam2 = CameraFactory::Create(c2);
    // Test name methods for WAC-UV
    cout << endl << endl << "Testing name methods ..." << endl;
    cout << "Spacecraft Name Long: " << cam2->spacecraftNameLong() << endl;
    cout << "Spacecraft Name Short: " << cam2->spacecraftNameShort() << endl;
    cout << "Instrument Name Long: " << cam2->instrumentNameLong() << endl;
    cout << "Instrument Name Short: " << cam2->instrumentNameShort() << endl << endl;
    
    // Test exceptions for determining names
    cout << endl << "Testing exceptions ..." << endl << endl;
    Cube test("$hayabusa/testData/st_2530292409_v.cub", "r");
    LroWideAngleCamera lCam(test);
  }
  catch(IException &e) {
    e.print();
  }
}

void TestLineSamp(Camera *cam, double samp, double line) {
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

