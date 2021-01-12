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

#include <QStringList>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "LroNarrowAngleCamera.h"
#include "Preference.h"
#include "Pvl.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

int main(void) {
  Preference::Preferences(true);

  /**
   *  LRO NAC: The line,samp to lat,lon to line,samp tolerance was increased for
   *    this camera model test. This test was re-written and should NOT be used
   *    as a template for other camera model unit tests.
   */
  cout << "Unit Test for LroNarrowAngleCamera..." << endl;
  try {
    double knownLat = -83.2598150072595899;
    double knownLon = 353.9497987082821737;
    Cube c("$lro/testData/M111607830RE_crop.cub", "r");
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
    // Test name methods
    cout << endl << endl << "Testing name methods ..." << endl;
    QStringList files;
    files.append("$lro/testData/M111607830RE_crop.cub");
    files.append("$lro/testData/M1153718003LE.reduced.cub");
    
    for (int i = 0; i < files.size(); i++) {
      Cube n(files[i], "r");
      Camera *nCam = CameraFactory::Create(n);
      cout << "Spacecraft Name Long: " << nCam->spacecraftNameLong() << endl;
      cout << "Spacecraft Name Short: " << nCam->spacecraftNameShort() << endl;
      cout << "Instrument Name Long: " << nCam->instrumentNameLong() << endl;
      cout << "Instrument Name Short: " << nCam->instrumentNameShort() << endl << endl;
    }
    
    // Test exception: camera is not a supported Kaguya camera
    cout << endl << "Testing exceptions:" << endl << endl;
    Cube test("$hayabusa/testData/st_2530292409_v.cub", "r");
    LroNarrowAngleCamera testCam(test);
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

