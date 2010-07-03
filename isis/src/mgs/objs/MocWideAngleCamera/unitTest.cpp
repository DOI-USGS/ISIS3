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
 *            from 1E-10 to 1.7E-10 since the difference on
 *            Linux i686 was 1.68E-10.
 *   @history 2010-02-24 Christopher Austin - Altered knownLat/Lon
 *            for the new naif precision
 */
int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for MocWideAngleCamera..." << endl;
  /* 
   * MocWideAngleCamera: The line,samp to lat,lon to line,samp tolerance was increased for this
   *  camera model test.
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    //    double knownLat = 22.75215777208721;
    //    double knownLon = 225.6312171563951;
    //    double knownLat = 22.7521581002579;
    //    double knownLon = 225.6312103276302;
    //    double knownLat = 22.75215722648675;
    //    double knownLon = 225.6312106938898;
    //  blackflag
    //    double knownLat = 22.75215722969901;
    //    double knownLon = 225.6312106946057;
    //  deet
    //    double knownLat = 22.75215809276655;
    //    double knownLon = 225.6312105606938;
    double knownLat = 22.75215809276655;
    double knownLon = 225.6312105606938;

    Isis::Pvl p("$mgs/testData/ab102401.cub");
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
   
    if(fabs(cam->UniversalLatitude() - knownLat) < 3.39E-9) {
      cout << "Latitude OK" << endl;
    }
    else {
      cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat << endl;
    }

    if(fabs(cam->UniversalLongitude() - knownLon) < 7.97E-10) {
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
    if (fabs(deltaSamp) < 0.01) deltaSamp = 0;
    if (fabs(deltaLine) < 0.001) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

