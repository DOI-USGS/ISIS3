// $Id: unitTest.cpp,v 1.6 2009/01/22 22:04:55 kbecker Exp $
using namespace std;

#include <iomanip>
#include <iostream>
#include "Camera.h"
#include "Filename.h"
#include "CameraFactory.h"
#include "iException.h"
#include "Preference.h"

void TestLineSamp(Isis::Camera *cam, double samp, double line);

int main (void)
{
  Isis::Preference::Preferences(true);

  cout << "Unit Test for IssNACamera..." << endl;
  /*
   * Sample/Line TestLineSamp points changed for the IssNACamera
   */
  try{

//    Isis::Pvl p("N1477312678_2.cub");
    Isis::Pvl p("$cassini/testData/N1525100863_2.cub");
    Isis::Camera *cam = Isis::CameraFactory::Create(p);
    cout << "Filename: " << Isis::Filename(p.Filename()).Name() << endl;
    cout << "CK Frame: " << cam->InstrumentRotation()->Frame() << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(4);
   
    // Test all four corners to make sure the conversions are right
    cout << "\nFor upper left corner ..." << endl;
    TestLineSamp(cam, 0.5, 0.5);

    cout << "\nFor upper right corner ..." << endl;
    TestLineSamp(cam, cam->Samples()+0.5, 0.5);

    cout << "\nFor lower left corner ..." << endl;
    TestLineSamp(cam, 0.5, cam->Lines()+0.5);

    cout << "\nFor lower right corner ..." << endl;
    TestLineSamp(cam, cam->Samples()+0.5, cam->Lines()+0.5);

    double samp = cam->Samples() / 2.0 + 0.5;
    double line = cam->Lines() / 2.0 + 0.5;
    cout << "\nFor center pixel position ..." << endl;
    TestLineSamp(cam, samp, line);

  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

void TestLineSamp(Isis::Camera *cam, double samp, double line) {
  cout << "Line, Sample: " << line << ", " << samp << endl;
  bool success = cam->SetImage(samp,line);

  if(success) {
    cout << "Lat, Long:    " << cam->UniversalLatitude() << ", " 
         << cam->UniversalLongitude() << endl;
    double westlon = -cam->UniversalLongitude();
    while(westlon < 0.0) westlon += 360.0; 
    cout << "WestLon:      " << westlon << endl;
  }

}
