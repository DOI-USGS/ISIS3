using namespace std;

#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for IdealCamera..." << endl;
  /*
   *  IdealCamera unit test tests two images instead of the typical one. Increased tolerances
   *  for line/samp->lat/lon->line/samp
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat[2] = {22.79610742949229,-82.44021272004072};
    double knownLon[2] = {225.0055153560127,75.88746680693571};
    char files[2][1024] = { "$base/testData/ab102401_ideal.cub", "$base/testData/f319b18_ideal.cub" };

    for(unsigned int i = 0; i < sizeof(knownLat)/sizeof(double); i++) {
      Isis::Pvl p(files[i]);
      Isis::Camera *cam = Isis::CameraFactory::Create(p);
      cout << setprecision(9);
      cout << "Testing image " << files[i] << " ..." << endl;
     
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
  
      if(abs(cam->UniversalLatitude() - knownLat[i]) < 1E-10) {
        cout << "Latitude OK" << endl;
      }
      else {
        cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat[i] << endl;
      }
  
      if(abs(cam->UniversalLongitude() - knownLon[i]) < 1E-10) {
        cout << "Longitude OK" << endl;
      }
      else {
        cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon[i] << endl;
      }

      cout << endl;
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
    if (fabs(deltaSamp) < 0.013) deltaSamp = 0;
    if (fabs(deltaLine) < 0.0016) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}
