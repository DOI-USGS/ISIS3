/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraFactory.h"
#include "IException.h"
#include "iTime.h"
#include "OsirisRexTagcamsCamera.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void testCamera(Cube &c, double sample, double line, double knownLat, double knownLon);
void testLineSamp(Camera *cam, double sample, double line);

/**
 * OSIRIS-REx Tagcams Camera model unit test for instruments NAVCam, NFTCam
 * (and perhaps someday, StowCam). 
 *
 * @author  2024-03-01 Ken Edmundson
 * @internal
 *   @history 2024-03-01 Ken Edmundson - Initial version. 
 * 
 */

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for OsirisRexTagcamsCamera..." << endl;
  try {
    cout << "\nTesting NAVCam (backwards compatibility)..." << endl;
    Cube navCube(FileName("$ISISROOT/../isis/tests/data/osirisRexImages/20200303T213031S138_ncm_L0-reduced.cub").expanded(), "r");    
 
    // checking at center of format of NavCam
    double knownLat = 49.7487786981275;
    double knownLon = 43.7549667753273;
    double sample = 129.5; 
    double line = 97.0;

    testCamera(navCube, sample, line, knownLat, knownLon);
    cout << "============================================================================" << endl;

    cout << "\nTesting NFTCam (backwards compatibility)..." << endl;
    Cube nftCube(FileName("$ISISROOT/../isis/tests/data/osirisRexImages/20201020T214241S004_nft_L0-reduced.cub").expanded(), "r");    
    
    // checking at center of format of NftCam
    knownLat = 53.7314045659365;
    knownLon = 45.4736806050086;
    sample = 129.5; 
    line = 97.0;

    testCamera(nftCube, sample, line, knownLat, knownLon);
    cout << "============================================================================" << endl;

    // TODO: COMPLETE IF/WHEN NAIF HAS PROVIDED KERNELS FOR STOWCAM. CURRENT IK IS LABELED AS
    //       PLACEHOLDER ONLY.

    cout << "\nTesting StowCam (backwards compatibility)..." << endl;
    cout << "\nTODO: COMPLETE IF/WHEN NAIF KERNELS AVAILABLE; CURRENT IK IS PLACEHOLDER." << endl;
/*
    Cube stowCube(placeholder, "r");    
    knownLat = 0.0;
    knownLon = 0.0;
    sample = 1296.0; 
    line = 972.0;
    testCamera(stowCube, sample, line, knownLat, knownLon);
*/
    cout << "============================================================================" << endl;
  }
  catch (IException &e) {
    cout << "Failed unitTest." << endl;
    e.print();
  }
}

void testCamera(Cube &cube, 
                double sample, double line,
                double knownLat, double knownLon) {

  OsirisRexTagcamsCamera *cam = (OsirisRexTagcamsCamera *) CameraFactory::Create(cube);
  cout << "FileName: " << FileName(cube.fileName()).name() << endl;
  cout << "NAIF Frame ID: " << cam->instrumentRotation()->Frame() << endl << endl;
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

  // Test Shutter Open/Close 
  const PvlGroup &inst = cube.label()->findGroup("Instrument", Pvl::Traverse);
  double exposureDuration = ((double) inst["ExposureDuration"])/1000;
  QString stime = inst["StartTime"];
  double et; // StartTime keyword is the center exposure time
  str2et_c(stime.toLatin1().data(), &et);
  pair <iTime, iTime> shuttertimes = cam->ShutterOpenCloseTimes(et, exposureDuration);
  cout << "Shutter open = " << shuttertimes.first.Et() << endl;
  cout << "Shutter close = " << shuttertimes.second.Et() << endl << endl;

  // Test all four corners to make sure the conversions are right
  cout << "For upper left corner ..." << endl;
  testLineSamp(cam, 1.0, 1.0);

  cout << "For upper right corner ..." << endl;
//  testLineSamp(cam, 2596.0, 1.0);
  testLineSamp(cam, 259.0, 1.0);

  cout << "For lower left corner ..." << endl;
//  testLineSamp(cam, 1.0, 1944.0);
  testLineSamp(cam, 1.0, 194.0);

  cout << "For lower right corner ..." << endl;
//  testLineSamp(cam, 2596.0, 1944.0);
  testLineSamp(cam, 259.0, 194.0);

  cout << "For known pixel position (" << sample << ", " << line << "..." << endl;
  if (!cam->SetImage(sample, line)) {
    throw IException(IException::Unknown, "ERROR setting image to known position.", _FILEINFO_);
  }
  if (abs(cam->UniversalLatitude() - knownLat) < 6E-14) {
    cout << "Latitude OK" << endl;
  }
  else {
    cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
  }
  if (abs(cam->UniversalLongitude() - knownLon) < 6E-14) {
    cout << "Longitude OK" << endl;
  }
  else {
    cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon << endl;
  }
}


void testLineSamp(Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);
  
  if (success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());    
  }

  if (success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if (fabs(deltaSamp) < 0.001) deltaSamp = 0.0;
    if (fabs(deltaLine) < 0.001) deltaLine = 0.0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = NO INTERSECTION" << endl;
    cout << "DeltaLine = NO INTERSECTION" << endl << endl;
  }
}

