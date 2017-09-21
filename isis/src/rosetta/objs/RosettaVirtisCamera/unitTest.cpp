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
#include <algorithm>

#include "Camera.h"
#include "CameraFactory.h"
#include "RosettaVirtisCamera.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

/**
 *
 * Unit test for DawnVirCamera.
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2018-08-28 Kris Becker - Initial unit test for Rosetta VIRTIS
 *            instrument
 */  
int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for RosettaVirtisCamera..." << endl;
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat = -13.317366927129520;
    double knownLon = 125.530724158825336;

    Cube c("$ISIS3DATA/rosetta/testData/I1_00237395857.cub", "r");
//    Cube c("./I1_00237395857.cub", "r");
    RosettaVirtisCamera *cam = (RosettaVirtisCamera *) CameraFactory::Create(c);
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
    //const PvlGroup &inst = p.findGroup("Instrument", Pvl::Traverse);
    std::pair< double, double > imgTimes = cam->StartEndEphemerisTimes();
    cout << "Start Time: " << imgTimes.first << "\n";
    cout << "End Time:   " << imgTimes.second << "\n";
    // Test all four corners to make sure the conversions are right
#if 1
    // Note - The limited data available at the time/scope this camera model
    // was developed (2017-08-28) does not fully support this test!!
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 119.0, 4.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, 135.0, 4.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 122.0, 9.0);

    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, 130.0, 9.0);
#endif

    double samp = 128.0;
    double line = 8.0;
    cout << "For center pixel position ..." << endl;

    if(!cam->SetImage(samp, line)) {
      cout << "ERROR" << endl;
      return 0;
    }

    if(abs(cam->UniversalLatitude() - knownLat) < 6E-12) {
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
    }

    if(abs(cam->UniversalLongitude() - knownLon) < 6E-12) {
      cout << "Longitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon << endl;
    }
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

