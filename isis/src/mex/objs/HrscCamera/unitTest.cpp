using namespace std;

#include <cmath>
#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);
/**
 * @internal 
 *   @history 2009-08-03 Jeannie Walldren - Changed known lat
 *            and lon.
 *   @history 2009-08-06 Jeannie Walldren - Added cmath include
 *            and changed calls to abs() to fabs() since the
 *            abs() function takes integer values while the
 *            fabs() takes floats. Changed Center Lat tolerance
 *            from 4E-10 to 4.1E-10 since the difference on
 *            Darwin powerpc was 4.07E-10.
 */
int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for HrscCamera..." << endl;
  /** HRSC: Corner tests offset a little, longitude tolerance increased.
   *  DeltaSamp/Line tolerances also increased.
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    //    double knownLat = -64.50313745696818;
    //    double knownLon = 43.52322196365473;
    //    double knownLat = -64.50313965814711;
    //    double knownLon = 43.5232222641892;
    //    double knownLat = -64.50313975280879;  //blackflag
    //    double knownLon = 43.52322225395123;
    //    double knownLat = -64.50312208873054;
    //    double knownLon = 43.52322362019364;
    //    double knownLat = -64.50312199406929;
    //    double knownLon = 43.52322363043155;
    //    double knownLat = -64.50312208873054;
    //    double knownLon = 43.52322362019364;
    double knownLat = -64.50314015330002;//blackflag
    double knownLon = 43.52322224602578;
    //    double knownLat = -64.50312208873054;//hiclops
    //    double knownLon = 43.52322362019364;

    Isis::Pvl p("$mex/testData/h2254_0000_s12.cub");
    Isis::Camera *cam = Isis::CameraFactory::Create(p);
    cout << setprecision(9);
   
    // Test all four corners to make sure the conversions are right
    cout << "For upper left corner ..." << endl;
    TestLineSamp(cam, 1.0, 1.0);

    cout << "For upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), 2.0);

    cout << "For lower left corner ..." << endl;
    TestLineSamp(cam, 1.0, cam->Lines());

    cout << "For lower right corner ..." << endl;
    TestLineSamp(cam, cam->Samples(), cam->Lines());

    double samp = cam->Samples() / 2;
    double line = cam->Lines() / 2;
    cout << "For center pixel position ..." << endl;

    if(!cam->SetImage(samp,line)) {
      std::cout << "ERROR" << std::endl;
      return 0;
    }

    if(fabs(cam->UniversalLatitude() - knownLat) < 1.81E-5) { 
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
    }

    if(fabs(cam->UniversalLongitude() - knownLon) < 1.4E-6) {
      cout << "Longitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon << endl;
    }
  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

void TestLineSamp(Isis::Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp,line);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if(success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if (fabs(deltaSamp) < 0.008) deltaSamp = 0;
    if (fabs(deltaLine) < 0.008) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

