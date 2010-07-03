using namespace std;

#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main(void) {
  Isis::Preference::Preferences(true);

  cout << "Unit Test for Mariner10Camera..." << endl;
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    char files[2][1024] = { "$mariner10/testData/27265.cub",
                            "$mariner10/testData/166474.cub"
                          };

    double knownLat[2] = { -27.5746680495404,
                           -22.58558961173848
                         };
    double knownLon[2] = { 312.1934068790935,
                           292.0697686733246
                         };

    vector< pair <int, int> > corners;

    //  27265.cub  Mariner A
    corners.push_back(std::make_pair(14, 1));
    corners.push_back(std::make_pair(831, 1));
    corners.push_back(std::make_pair(9, 700));
    corners.push_back(std::make_pair(829, 700));
    //  166474.cub Mariner B
    corners.push_back(std::make_pair(36, 1));
    corners.push_back(std::make_pair(829, 1));
    corners.push_back(std::make_pair(55, 700));
    corners.push_back(std::make_pair(830, 700));


    for(unsigned int i = 0; i < sizeof(knownLat) / sizeof(double); i++) {
      //    Isis::Pvl p("$mariner10/testData/27265.cub");
      Isis::Pvl p(files[i]);
      Isis::Camera *cam = Isis::CameraFactory::Create(p);
      cout << setprecision(9);
      cout << "Testing image " << files[i] << " ..." << endl;

      // Test all four corners to make sure the conversions are right
      cout << "For upper left corner ..." << endl;
      TestLineSamp(cam, corners[i*4].first, corners[i*4].second);

      cout << "For upper right corner ..." << endl;
      TestLineSamp(cam, corners[i*4+1].first, corners[i*4+1].second);

      cout << "For lower left corner ..." << endl;
      TestLineSamp(cam, corners[i*4+2].first, corners[i*4+2].second);

      cout << "For lower right corner ..." << endl;
      TestLineSamp(cam, corners[i*4+3].first, corners[i*4+3].second);

      double samp = cam->Samples() / 2;
      double line = cam->Lines() / 2;
      cout << "For center pixel position ..." << endl;

      if(!cam->SetImage(samp, line)) {
        std::cout << "ERROR" << std::endl;
        return 0;
      }

      if(abs(cam->UniversalLatitude() - knownLat[i]) < 1E-10) {
        cout << "Latitude OK" << endl;
      }
      else {
        cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat[i] << endl;
      }

      if(abs(cam->UniversalLongitude() - knownLon[i]) < 2E-10) {
        cout << "Longitude OK" << endl;
      }
      else {
        cout << setprecision(16) << "Longitude off by: " << cam->UniversalLongitude() - knownLon[i] << endl;
      }
    }
  }
  catch(Isis::iException &e) {
    e.Report();
  }
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

