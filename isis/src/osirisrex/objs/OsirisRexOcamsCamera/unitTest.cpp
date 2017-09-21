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
#include "IException.h"
#include "iTime.h"
#include "OsirisRexOcamsCamera.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void testCamera(Cube &c, double sample, double line, double knownLat, double knownLon);
void testLineSamp(Camera *cam, double sample, double line);

/**
 * This is the unit test for the Osiris Rex Camera model. 
 *
 * @author  2015-11-10 Stuart C. Sides
 * @internal
 *   @history 2015-11-10 Stuart C. Sides - Original version (PolyCam test).
 *   @history 2016-12-27 Jeannie Backer - Added test for MapCam. Added placeholder
 *                           for SamCam test (when we have an image we can spiceinit).
 *   @history 2017-08-29 Jeannie Backer - Added test for PolyCam and MapCam with
 *                           PolyCamFocusPositionNaifId keyword.
 *   @history 2017-09-18 Kristin Berry - Updated known latitudes and longitudes for the addition of
 *                           the distortion model. (For non-backwards compatibility MapCam cube
 *                           only, since we do not yet have a PolyCam cube with a motor position 
 *                           that we have a distortion solution for.) 
 * 
 */

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for OsirisRexOcamsCamera..." << endl;
  try {

    cout << "\nTesting PolyCam (backwards compatibility)..." << endl;
    Cube polyCamCube("$osirisrex/testData/2019-01-13T23_36_05.000_PCAM_L2b_V001.cub", "r");
    double knownLat = 13.9465663689936950;
    double knownLon = 349.0213035062322433;
    double sample = 512.0; 
    double line = 512.0;
    testCamera(polyCamCube, sample, line, knownLat, knownLon);
    cout << "\nTesting PolyCam (with PolyCamFocusPositionNaifId keyword)..." << endl;
    Cube polyCamCube2("$osirisrex/testData/20190113T191852S740_pol_iofL2pan_V001.cub", "r");
    knownLat = -5.5191879351483450;
    knownLon = 349.6939492565607566;
    sample = 512.0; 
    line = 512.0;
    testCamera(polyCamCube2, sample, line, knownLat, knownLon);
    cout << "============================================================================" << endl;

    cout << "\nTesting MapCam (backwards compatibility)..." << endl;
    Cube mapCamCube("$osirisrex/testData/D19030320000.cub", "r");
    knownLat = 73.9976065262802933;
    knownLon = 149.3814386120742768;
    sample = 512.0; 
    line = 512.0;
    testCamera(mapCamCube, sample, line, knownLat, knownLon);
    cout << "\nTesting MapCam (with PolyCamFocusPositionNaifId keyword)..." << endl;
    Cube mapCamCube2("$osirisrex/testData/20190303T100344S990_map_iofL2pan_V001.cub", "r");
    knownLat = -19.2946930665326732;
    knownLon = 145.9510736765638512;
    sample = 512.0; 
    line = 512.0;
    testCamera(mapCamCube2, sample, line, knownLat, knownLon);
    cout << "============================================================================" << endl;

/*
    cout << "\nTesting SamCam..." << endl;
    Cube samCamCube("$osirisrex/testData/20141111T202650_SCAM_L2_V001_SCAM.cub", "r");
    knownLat = 0.0;
    knownLon = 0.0;
    sample = 512.0; 
    line = 512.0;
    testCamera(samCamCube, sample, line, knownLat, knownLon);
    cout << "============================================================================" << endl;
*/
  }
  catch (IException &e) {
    cout << "Failed unitTest." << endl;
    e.print();
  }
}

void testCamera(Cube &cube, 
                double sample, double line,
                double knownLat, double knownLon) {

  OsirisRexOcamsCamera *cam = (OsirisRexOcamsCamera *) CameraFactory::Create(cube);
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
  testLineSamp(cam, 1024.0, 1.0);

  cout << "For lower left corner ..." << endl;
  testLineSamp(cam, 1.0, 1024.0);

  cout << "For lower right corner ..." << endl;
  testLineSamp(cam, 1024.0, 1024.0);

  cout << "For known pixel position ..." << endl;
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

