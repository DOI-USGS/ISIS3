using namespace std;

#include <iomanip>
#include "iException.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit Test for CTXCamera..." << endl;

  /** CTX: The line,samp to lat,lon to line,samp tolerance was increased for this
   *  camera model test.
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    double knownLat[2] = {-22.00025221053369, -15.08767100514901};
    double knownLon[2] = {307.9151038426381, 309.8666470890192};
    char files[2][1024] = { "$mro/testData/ctx_pmoi_i_00003.bottom.cub", "$mro/testData/ctx_pmoi_i_00003.top.cub" };

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
    if (fabs(deltaSamp) < 0.01) deltaSamp = 0;
    if (fabs(deltaLine) < 0.01) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}
