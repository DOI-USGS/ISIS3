/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iomanip>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "KaguyaMiCamera.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "Unit Test for KabuyaMiCamera..." << endl;

  /** CTX: The line,samp to lat,lon to line,samp tolerance was increased for this
   *  camera model test.
   */
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat[4] = { -12.0400820752276996, 47.7445483329470406,
                            60.8041933170744215, 60.1567063916710580};
    double knownLon[4] = { 355.7272261079595523, 42.9611485167199660,
                           135.3886983694549713, 135.3809757236753057};

    char files[4][1024] = { "$ISISTESTDATA/isis/src/kaguya/unitTestData/MI_VIS.cub", "$ISISTESTDATA/isis/src/kaguya/unitTestData/MI_NIR.cub",
                            "$ISISTESTDATA/isis/src/kaguya/unitTestData/MVA_2B2_01_01228N608E1354.cub",
                            "$ISISTESTDATA/isis/src/kaguya/unitTestData/MNA_2B2_01_01228N602E1354.cub"};

    for (unsigned int i = 0; i < sizeof(knownLat) / sizeof(double); i++) {
      Cube c(files[i], "r");
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
      TestLineSamp(cam, cam->Samples(), 1.0);

      cout << "For lower left corner ..." << endl;
      TestLineSamp(cam, 1.0, cam->Lines());

      cout << "For lower right corner ..." << endl;
      TestLineSamp(cam, cam->Samples(), cam->Lines());

      double samp = cam->Samples() / 2;
      double line = cam->Lines() / 2;
      cout << "For center pixel position ..." << endl;

      if(!cam->SetImage(samp, line)) {
        cout << "ERROR" << endl;
        return 0;
      }

      if(abs(cam->UniversalLatitude() - knownLat[i]) < 1E-10) {
        cout << "Latitude OK" << endl;
      }
      else {
        //cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat[i] << endl;
        cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() << endl;
      }

      if(abs(cam->UniversalLongitude() - knownLon[i]) < 1E-10) {
        cout << "Longitude OK" << endl;
      }
      else {
//        cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon[i] << endl;
        cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude()  << endl;
      }
      cout << endl << "--------------------------------------------" << endl;
    }

    // Test exception: camera is not a supported Kaguya camera
    cout << endl << "Testing exceptions:" << endl << endl;
    Cube test("$ISISTESTDATA/isis/src/hayabusa/unitTestData/st_2530292409_v.cub", "r");
    KaguyaMiCamera kCam(test);
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
