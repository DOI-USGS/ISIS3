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
#include "Filename.h"
#include "IException.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main(void) {
  Isis::Preference::Preferences(true);

  cout << "Unit Test for MiniRFCamera..." << endl;
 // try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat = 85.5973879396895398;
    double knownLon = 264.7361454607174664;

    Pvl p("$chan1/testData/FSR_CDR_LV1_01801_0R.cub");
    Camera *cam = Isis::CameraFactory::Create(p);
    cout << "Filename: " << Filename(p.Filename()).Name() << endl;
    cout << "CK Frame: " << cam->InstrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
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

    if(!cam->SetImage(samp, line)) {
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

    // Test kernel ID messages
    cout << endl << "Kernel ID error messages: " << endl;
    try{ cam->CkFrameId(); }
    catch (IException e){ e.print(); }
    try{ cam->CkReferenceId(); }
    catch (IException e){ e.print(); }
    try{ cam->SpkTargetId(); }
    catch (IException e){ e.print(); }
    try{ cam->SpkReferenceId(); }
    catch (IException e){ e.print(); }

 // }
//  catch(Isis::IException &e) {
//    e.print();
//  }
}

void TestLineSamp(Isis::Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if(success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if(fabs(deltaSamp) < 0.001) deltaSamp = 0;
    if(fabs(deltaLine) < 0.001) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

