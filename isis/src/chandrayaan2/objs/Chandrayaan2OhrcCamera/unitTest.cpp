/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);


int main(void) {
  Preference::Preferences(true);


  cout << "Unit test for Chandrayaan2OHRC camera." << endl;
  
  try {
    
    std::string pref = "ch2_ohr_nrp_20200827T0226453039_d_img_d18_crop"; 
    std::string path =  "../../../../tests/data/chandrayaan2/" + pref + ".cub";
    Cube c(path.c_str(), "r");
    Camera *cam = CameraFactory::Create(c);
    cout << "FileName: " << FileName(c.fileName()).name() << endl;
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
    TestLineSamp(cam, 1.0, 1.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, 90.5, 1.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 1.0, 90.5);

    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, 90.5, 90.5);

    cout << "For center pixel position ..." << endl;
    double samp = 49.5;
    double line = 49.5;
    TestLineSamp(cam, samp, line);

    if (!cam->SetImage(samp, line)) {
      cout << "ERROR" << endl;
      return 0;
    }

    std::cout << "Universal latitude: " << cam->UniversalLatitude() << endl;
    std::cout << "Universal longitude: " << cam->UniversalLongitude() << endl;
                        
    cout << "RightAscension: " << cam->RightAscension() << endl;
    cout << "Declination: " << cam->Declination() << endl;

  } catch (IException &e) {
    e.print();
  }
}


void TestLineSamp(Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);

  if (success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if (success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if (fabs(deltaSamp) < 1.0E-4) deltaSamp = 0;
    if (fabs(deltaLine) < 1.0E-4) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

