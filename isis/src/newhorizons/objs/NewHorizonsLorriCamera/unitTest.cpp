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
#include "NewHorizonsLorriCamera.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);



int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for NewHorizonsLorriCamera..." << endl;
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat = -8.2967175607848702;
    double knownLon = 210.1990629768775705;
    // double knownLat = 0.0;
    // double knownLon = 0.0;

    Cube c("$newhorizons/testData/lor_0034821014_0x630_sci_1_v2.cub", "r");
    NewHorizonsLorriCamera *cam = (NewHorizonsLorriCamera *) CameraFactory::Create(c);
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

    // Test Shutter Open/Close 
    const PvlGroup &inst = c.label()->findGroup("Instrument", Pvl::Traverse);
    double exposureDuration = ((double) inst["ExposureDuration"])/1000;
    QString stime = inst["StartTime"];
    double et; // StartTime keyword is the center exposure time
    str2et_c(stime.toAscii().data(), &et);
    pair <iTime, iTime> shuttertimes = cam->ShutterOpenCloseTimes(et, exposureDuration);
    cout << "Shutter open = " << shuttertimes.first.Et() << endl;
    cout << "Shutter close = " << shuttertimes.second.Et() << endl << endl;

    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 255.0, 490.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, 426.0, 473.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 258.0, 578.0);

    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, 412.0, 594.0);

    double samp = 358.0;
    double line = 534.0;
    cout << "For center pixel position ..." << endl;

    if (!cam->SetImage(samp, line)) {
      cout << "ERROR" << endl;
      return 0;
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
  catch (IException &e) {
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
    if (fabs(deltaSamp) < 0.001) deltaSamp = 0.0;
    if (fabs(deltaLine) < 0.001) deltaLine = 0.0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

