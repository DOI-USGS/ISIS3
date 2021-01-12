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

#include <QList>
#include <QString>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "VoyagerCamera.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for Voyager..." << endl;
  /*
   * Sample/Line TestLineSamp points were changed for this test.
   * The tolerance for Sample/Line to Lat/Lon and back was increased
   * for this test.
   */
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat = -1.03098148697020941;
    double knownLon = 82.0423364316989279;
    
    Cube c("$voyager1/testData/c1639118.imq.cub", "r");
    VoyagerCamera *cam = (VoyagerCamera *) CameraFactory::Create(c);
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
    
    // Test Shutter Open/Close 
    const PvlGroup &inst = c.label()->findGroup("Instrument", Pvl::Traverse);
    double exposureDuration = ((double) inst["ExposureDuration"])/1000; 
    QString stime = inst["StartTime"];
    double et; // StartTime keyword is the center exposure time
    str2et_c(stime.toLatin1().data(), &et);
    pair <iTime, iTime> shuttertimes = cam->ShutterOpenCloseTimes(et, exposureDuration);
    cout << "Shutter open = " << shuttertimes.first.Et() << endl;
    cout << "Shutter close = " << shuttertimes.second.Et() << endl << endl;
    
    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 1.0, 1.0);
    
    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples() - 5.0, 16.0);
    
    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 12.0, cam->Lines() - 12.0);
    
    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, cam->Samples() - 4.0, cam->Lines());
    
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
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat 
           << endl;
    }
    
    if(abs(cam->UniversalLongitude() - knownLon) < 1E-10) {
      cout << "Longitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon
           << endl;
    }
    
    // Test name methods
    cout << endl << "Testing name methods" << endl << endl;
    QList<QString> files;
    files.append("$voyager1/testData/c1639118.imq.cub"); // Voyager1 NAC
    files.append("$voyager1/testData/c1639241.cropped.cub"); // Voyager1 WAC
    files.append("$voyager2/testData/c2065022.cropped.cub"); // Voyager2 NAC
    files.append("$voyager2/testData/c4397840.cropped.cub"); // Voyager2 WAC
  
    for (int i = 0; i < files.size(); i++) {
      Cube cu(files[i], "r");
      VoyagerCamera *vCam = (VoyagerCamera *) CameraFactory::Create(cu);
      cout << "Spacecraft Name Long: " << vCam->spacecraftNameLong() << endl;
      cout << "Spacecraft Name Short: " << vCam->spacecraftNameShort() << endl;
      cout << "Instrument Name Long: " << vCam->instrumentNameLong() << endl;
      cout << "Instrument Name Short: " << vCam->instrumentNameShort() << endl << endl;
    }
    
    // Test exception: camera is not a supported Kaguya camera
    cout << endl << "Testing exceptions:" << endl << endl;
    Cube test("$hayabusa/testData/st_2530292409_v.cub", "r");
    VoyagerCamera testCam(test);
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
    if(fabs(deltaSamp) < 0.05) deltaSamp = 0;
    if(fabs(deltaLine) < 0.04) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

