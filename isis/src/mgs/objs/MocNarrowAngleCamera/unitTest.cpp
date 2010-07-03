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
 *            from 1E-10 to 2E-10 since the difference on Darwin powerpc
 *            was 1.93E-10.  Changed Center Lon tolerance from
 *            1E-10 to 1.1E-10 since the difference on Darwin powerpc
 *            was 1.03E-10.
 *   @history 2010-02-24 Christopher Austin - Changed the knowLat/Lon
 *            in accordence with the system changes
 */

int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for MocNarrowAngleCamera..." << endl;
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    //    double knownLat = -9.931519304695483;
    //    double knownLon = 286.6184572896669;
    //    double knownLat = -9.931519304671058;
    //    double knownLon = 286.6184572896659;
    //  blackflag
    //    double knownLat = -9.931519304119526;
    //    double knownLon = 286.6184572896647;
    //  deet (current)
    //    double knownLat = -9.931519304735847;
    //    double knownLon = 286.6184572896974;
    double knownLat = -9.931519304735847;
    double knownLon = 286.6184572896974;

    //Isis::Pvl p("$mgs/testData/lub0428b.cub");
    Isis::Pvl p("$mgs/testData/fha00491.lev1.cub");
    Isis::Camera *cam = Isis::CameraFactory::Create(p);
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

    if(!cam->SetImage(samp,line)) {
      std::cout << "ERROR" << std::endl;
      return 0;
    }

    if(fabs(cam->UniversalLatitude() - knownLat) < 2E-10) {
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
    }

    if(fabs(cam->UniversalLongitude() - knownLon) < 1.1E-10) {
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
    if (fabs(deltaSamp) < 0.001) deltaSamp = 0;
    if (fabs(deltaLine) < 0.001) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

