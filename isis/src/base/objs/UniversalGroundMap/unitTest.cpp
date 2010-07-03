#include <iostream>
#include <iomanip>
#include "iException.h"
#include "UniversalGroundMap.h"
#include "Camera.h"
#include "Pvl.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try{
    cout << "UnitTest for Universal Ground Map" << endl;

    cout << "  Testing Camera Model..." << endl;
    Isis::Pvl p("$viking2/testData/f348b26.cub");
    Isis::UniversalGroundMap ugm(p);;
    cout << setprecision(9);

    cout << "Is Projection? = " << ugm.HasProjection() << endl << endl;

    // Test all four corners to make sure the conversions are right
    ugm.SetImage(1.0, 1.0);
    cout << "For (1.0, 1.0) ..." << endl;
    cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;
   
    ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude());
    cout << "Sample = " << ugm.Sample() << endl;
    cout << "Line = " << ugm.Line() << endl << endl;
   
    ugm.SetImage(1204, 1.0);
    cout << "For (1204, 1.0) ..." << endl;
    cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;
   
    ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude());
    cout << "Sample = " << ugm.Sample() << endl;
    cout << "Line = " << ugm.Line() << endl << endl;
   
    ugm.SetImage(1.0, 1056);
    cout << "For (1.0, 1056) ..." << endl;
    cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;
   
    ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude());
    cout << "Sample = " << ugm.Sample() << endl;
    cout << "Line = " << ugm.Line() << endl << endl;
   
    ugm.SetImage(1204, 1056);
    cout << "For (1204, 1056) ..." << endl;
    cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;
   
    ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude());
    cout << "Sample = " << ugm.Sample() << endl;
    cout << "Line = " << ugm.Line() << endl << endl;



    cout << "  Testing Projection..." << endl;
    Isis::Pvl p2("$base/dems/molaMarsPlanetaryRadius0001.cub");
    Isis::UniversalGroundMap ugm2(p2);
    cout << "Is Projection? = " << ugm2.HasProjection() << endl << endl;
  
    // Test all four corners to make sure the conversions are right
    ugm2.SetImage(1.0, 1.0);
    cout << "For (1.0, 1.0) ..." << endl;
    cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

    ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude());
    cout << "Sample = " << ugm2.Sample() << endl;
    cout << "Line = " << ugm2.Line() << endl << endl;

    ugm2.SetImage(23040, 1.0);
    cout << "For (23040, 1.0) ..." << endl;
    cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

    ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude());
    cout << "Sample = " << ugm2.Sample() << endl;
    cout << "Line = " << ugm2.Line() << endl << endl;

    ugm2.SetImage(1.0, 11520);
    cout << "For (1.0, 11520) ..." << endl;
    cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

    ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude());
    cout << "Sample = " << ugm2.Sample() << endl;
    cout << "Line = " << ugm2.Line() << endl << endl;

    ugm2.SetImage(23040, 11520);
    cout << "For (23040, 11520) ..." << endl;
    cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

    ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude());
    cout << "Sample = " << ugm2.Sample() << endl;
    cout << "Line = " << ugm2.Line() << endl << endl;

    cout << "  Testing Camera Model and Projection..." << endl;
    Isis::Pvl p3("$mgs/testData/m0402852.cub");
    Isis::UniversalGroundMap ugm3(p3);
    cout << "Is Projection? = " << ugm3.HasProjection() << endl << endl;

    // Test all four corners to make sure the conversions are right
    ugm3.SetImage(1.0, 1.0);
    cout << "For (1.0, 1.0) ..." << endl;
    cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

    ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude());
    cout << "Sample = " << ugm3.Sample() << endl;
    cout << "Line = " << ugm3.Line() << endl << endl;

    ugm3.SetImage(1721, 1.0);
    cout << "For (1721, 1.0) ..." << endl;
    cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

    ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude());
    cout << "Sample = " << ugm3.Sample() << endl;
    cout << "Line = " << ugm3.Line() << endl << endl;

    ugm3.SetImage(1.0, 667);
    cout << "For (1.0, 667) ..." << endl;
    cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

    ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude());
    cout << "Sample = " << ugm3.Sample() << endl;
    cout << "Line = " << ugm3.Line() << endl << endl;

    ugm3.SetImage(1721, 667);
    cout << "For (1721, 667) ..." << endl;
    cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
    cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

    ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude());
    cout << "Sample = " << ugm3.Sample() << endl;
    cout << "Line = " << ugm3.Line() << endl << endl;

  }
  catch (Isis::iException &e) {
   e.Report();
 }
}







