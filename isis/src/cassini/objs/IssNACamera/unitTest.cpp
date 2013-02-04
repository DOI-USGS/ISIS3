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
// $Id: unitTest.cpp,v 1.6 2009/01/22 22:04:55 kbecker Exp $
#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "IssNACamera.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for IssNACamera..." << endl;
  /*
   * Sample/Line TestLineSamp points changed for the IssNACamera. This test has
   *   been significantly modified for this camera and should not be used
   *   as a template.
   */
  try {

    Pvl p("$cassini/testData/N1525100863_2.cub");
    IssNACamera *cam = (IssNACamera *) CameraFactory::Create(p);
    cout << "FileName: " << FileName(p.FileName()).name() << endl;
    cout << "CK Frame: " << cam->instrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(9);

    // Test kernel IDs
    cout << "Kernel IDs: " << endl;
    cout << "CK Frame ID = " << cam->CkFrameId() << endl;
    cout << "CK Reference ID = " << cam->CkReferenceId() << endl;
    cout << "SPK Target ID = " << cam->SpkTargetId() << endl;
    cout << "SPK Reference ID = " << cam->SpkReferenceId() << endl << endl;

    // Test Shutter Open/Close 
    const PvlGroup &inst = p.FindGroup("Instrument", Pvl::Traverse);
    double exposureDuration = ((double) inst["ExposureDuration"])/1000; 
    QString stime = inst["StartTime"];
    double et; // StartTime keyword is the center exposure time
    str2et_c(stime.toAscii().data(), &et);
    pair <iTime, iTime> shuttertimes = cam->ShutterOpenCloseTimes(et, exposureDuration);
    cout << "Shutter open = " << shuttertimes.first.Et() << endl;
    cout << "Shutter close = " << shuttertimes.second.Et() << endl << endl;

    // Test all four corners to make sure the conversions are right
    cout << "\nFor upper left corner ..." << endl;
    TestLineSamp(cam, 0.5, 0.5);

    cout << "\nFor upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples() + 0.5, 0.5);

    cout << "\nFor lower left corner ..." << endl;
    TestLineSamp(cam, 0.5, cam->Lines() + 0.5);

    cout << "\nFor lower right corner ..." << endl;
    TestLineSamp(cam, cam->Samples() + 0.5, cam->Lines() + 0.5);

    double samp = cam->Samples() / 2.0 + 0.5;
    double line = cam->Lines() / 2.0 + 0.5;
    cout << "\nFor center pixel position ..." << endl;
    TestLineSamp(cam, samp, line);
  }
  catch(IException &e) {
    e.print();
  }
}

void TestLineSamp(Camera *cam, double samp, double line) {
  cout << "Line, Sample: " << line << ", " << samp << endl;
  bool success = cam->SetImage(samp, line);

  if(success) {
    cout << "Lat, Long:    " << cam->UniversalLatitude() << ", "
         << cam->UniversalLongitude() << endl;
    double westlon = -cam->UniversalLongitude();
    while(westlon < 0.0) westlon += 360.0;
    cout << "WestLon:      " << westlon << endl;
  }
  else {
    cout << "Point not on planet!\n";
  }
}
