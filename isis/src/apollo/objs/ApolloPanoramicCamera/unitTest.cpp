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

#include "ApolloPanoramicCamera.h"
#include "ApolloPanoramicDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

int main(void) {
  Preference::Preferences(true);
  cout << "Unit Test for ApolloCamera..." << endl;
  try {

 

    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat = 18.1052476112276679;
    double knownLon = 6.3330663560966673;

    Cube cubeTL(FileName("$apollo15/testData/TL.cub").expanded(), "r");
    ApolloPanoramicCamera *camTL = (ApolloPanoramicCamera *) CameraFactory::Create(cubeTL);

    Cube cubeBL(FileName("$apollo15/testData/BL.cub").expanded(), "r");
    ApolloPanoramicCamera *camBL = (ApolloPanoramicCamera *) CameraFactory::Create(cubeBL);

    Cube cubeM(FileName("$apollo15/testData/M.cub").expanded(), "r");
    ApolloPanoramicCamera *camM = (ApolloPanoramicCamera *) CameraFactory::Create(cubeM);

    Cube cubeTR(FileName("$apollo15/testData/TR.cub").expanded(), "r");
    ApolloPanoramicCamera *camTR = (ApolloPanoramicCamera *) CameraFactory::Create(cubeTR);

    Cube cubeBR(FileName("$apollo15/testData/BR.cub").expanded(), "r");
    ApolloPanoramicCamera *camBR = (ApolloPanoramicCamera *) CameraFactory::Create(cubeBR);

    cout << "FileName: " << FileName(cubeTL.fileName()).name() << endl;
    cout << "CK Frame: " << camTL->instrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(9);

    // Test kernel IDs
    cout << "Kernel IDs: " << endl;
    cout << "CK Frame ID = " << camTL->CkFrameId() << endl;
    cout << "CK Reference ID = " << camTL->CkReferenceId() << endl;
    cout << "SPK Target ID = " << camTL->SpkTargetId() << endl;
    cout << "SPK Reference ID = " << camTL->SpkReferenceId() << endl << endl;
    
         
    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(camTL, 1.0, 1.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(camTR, 1.0, 1.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(camBL, 1.0, 1.0);

    cout << "For lower right corner ..." << endl;
    TestLineSamp(camBR, 1.0, 1.0);

     cout << "For center pixel position ..." << endl;

    if(!camM->SetImage(1.0, 1.0)) {
      cout << "ERROR" << endl;
      return 0;
    }

    if(abs(camM->UniversalLatitude() - knownLat) < 1E-10) {
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << camM->UniversalLatitude() - knownLat << endl;
    }

    if(abs(camM->UniversalLongitude() - knownLon) < 1E-10) {
      cout << "Longitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Longitude off by: " << camM->UniversalLongitude() - knownLon << endl;
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
    cout << "DeltaLine = " << deltaLine << endl;
  }
  else {
    cout << "DeltaSample = ERROR ";
    cout << "DeltaLine = ERROR" << endl;
  }
}

