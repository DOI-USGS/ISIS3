using namespace std;

#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);

/**
 * @internal 
 *   @history 2009-08-19 Tracie Sucharski, Added all new tests, including
 *                         ir and vis, normal and hires modes with offsets.
 */
int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for VimsCamera..." << endl;
  /*
   * Sample/Line TestLineSamp points changed for the VimsCamera,
   *   tolerance increased
   */
  try{
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.

    //  For vims, testing ir and vis, hires and normal with offsets and swath
    //   sizes that are not 1,1,64,64 (full field of view)
    //  Ir Normal full field of view:                   CM_1515951157_1.ir.cub
    //  Ir Normal partial field of view (17,17,48,48):  CM_1514390782_1.ir.cub
    //  Vis Normal partial field of view (17,17,48,48): CM_1514390782_1.vis.cub
    //  Ir Hires partial field of view (17,26,48,36):   CM_1515945709_1.ir.cub
    //  Vis Hires partial field of view (17,26,48,36):  CM_1515945709_1.vis.cub
    char files[5][1024] = { "$cassini/testData/CM_1515951157_1.ir.cub",
                            "$cassini/testData/CM_1514390782_1.ir.cub",
                            "$cassini/testData/CM_1514390782_1.vis.cub",
                            "$cassini/testData/CM_1515945709_1.ir.cub",
                            "$cassini/testData/CM_1515945709_1.vis.cub" };

    double knownLat[5] = { 0.238187729668092,
                            -34.842880495045549,
                            -41.440694242217511,
                            -42.375769525935951,
                            -37.412633452415633 };
    double knownLon[5] = { 198.059202300647257,
                            123.561260864268348,
                            131.725836878894256,
                            202.623084593112708,
                            213.598346090590496 };

    vector< pair <int,int > > corners;
    //  CM_1515951157_1.ir.cub
    corners.push_back(std::make_pair(25,30));                             
    corners.push_back(std::make_pair(40,30));                             
    corners.push_back(std::make_pair(25,45));                             
    corners.push_back(std::make_pair(40,45));                             
    //  CM_1514390782_1.ir.cub
    corners.push_back(std::make_pair(22,20));                             
    corners.push_back(std::make_pair(40,20));                             
    corners.push_back(std::make_pair(22,33));                             
    corners.push_back(std::make_pair(40,33));                             

    //  CM_1514390782_1.vis.cub
    corners.push_back(std::make_pair(23,20));                             
    corners.push_back(std::make_pair(40,20));                             
    corners.push_back(std::make_pair(23,33));                             
    corners.push_back(std::make_pair(40,33));                             

    //  CM_1515945709_1.ir.cub
    corners.push_back(std::make_pair(21,14));                             
    corners.push_back(std::make_pair(48,14));                             
    corners.push_back(std::make_pair(23,24));                             
    corners.push_back(std::make_pair(48,26));                             

    //  CM_1515945709_1.vis.cub
    corners.push_back(std::make_pair(26,8));
    corners.push_back(std::make_pair(36,8));
    corners.push_back(std::make_pair(26,29));                             
    corners.push_back(std::make_pair(36,29));                             

    for (unsigned int i = 0; i < sizeof(knownLat)/sizeof(double); i++) {
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

      if (!cam->SetImage(samp,line)) {
        std::cout << "ERROR" << std::endl;
        return 0;
      }

      if (abs(cam->UniversalLatitude() - knownLat[i]) < 1E-8) {
        cout << "Latitude OK" << endl;
      }
      else {
        cout << setprecision(16) << "Latitude off by: " << cam->UniversalLatitude() - knownLat[i] << endl;
      }

      if (abs(cam->UniversalLongitude() - knownLon[i]) < 1E-8) {
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
    if (fabs(deltaSamp) < 0.196) deltaSamp = 0;
    if (fabs(deltaLine) < 0.196) deltaLine = 0;
    cout << "DeltaSample = " << deltaSamp << endl;
    cout << "DeltaLine = " << deltaLine << endl << endl;
  }
  else {
    cout << "DeltaSample = ERROR" << endl;
    cout << "DeltaLine = ERROR" << endl << endl;
  }
}

