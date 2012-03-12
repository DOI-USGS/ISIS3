#include <iostream>
#include <iomanip>

#include "Camera.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Pvl.h"
#include "Preference.h"
#include "SurfacePoint.h"
#include "UniversalGroundMap.h"


using namespace std;
using namespace Isis;


int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    cout << "UnitTest for Universal Ground Map" << endl;

    cout << "  Testing Camera Model..." << endl;
    Isis::Pvl p("$viking2/testData/f348b26.cub");
    Isis::UniversalGroundMap ugm(p);;
    cout << setprecision(9);

    cout << "Is Projection? = " << ugm.HasProjection() << endl << endl;

    // Test all four corners to make sure the conversions are right
    cout << "For (1.0, 5.0) ..." << endl;
    if (ugm.SetImage(1.0, 5.0)) {
      cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;

      if (ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude())) {
        cout << "Sample = " << ugm.Sample() << endl;
        cout << "Line = " << ugm.Line() << endl << endl;
      }
      if (ugm.SetGround(
          SurfacePoint(Latitude(ugm.UniversalLatitude(), Angle::Degrees),
                       Longitude(ugm.UniversalLongitude(), Angle::Degrees),
                       ugm.Camera()->LocalRadius()))) {
        cout << "Sample = " << ugm.Sample() << endl;
        cout << "Line = " << ugm.Line() << endl << endl;
      }
    }

    cout << "For (1204, 5.0) ..." << endl;
    if (ugm.SetImage(1204, 5.0)) {
      cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;

      if (ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude())) {
        cout << "Sample = " << ugm.Sample() << endl;
        cout << "Line = " << ugm.Line() << endl << endl;
      }
    }

    cout << "For (1.0, 1056) ..." << endl;
    if (ugm.SetImage(1.0, 1056)) {
      cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;

      if (ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude())) {
        cout << "Sample = " << ugm.Sample() << endl;
        cout << "Line = " << ugm.Line() << endl << endl;
      }
    }

    cout << "For (1204, 1056) ..." << endl;
    if (ugm.SetImage(1204, 1056)) {
      cout << "Universal Latitude = " << ugm.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm.UniversalLongitude() << endl;

      if (ugm.SetUniversalGround(ugm.UniversalLatitude(), ugm.UniversalLongitude())) {
        cout << "Sample = " << ugm.Sample() << endl;
        cout << "Line = " << ugm.Line() << endl << endl;
      }
    }

    cout << "  Testing Projection..." << endl;
    Isis::Pvl p2("$base/dems/molaMarsPlanetaryRadius0001.cub");
    Isis::UniversalGroundMap ugm2(p2);
    cout << "Is Projection? = " << ugm2.HasProjection() << endl << endl;

    // Test all four corners to make sure the conversions are right
    cerr << "For (2.0, 5.0) ..." << endl;
    if (ugm2.SetImage(2.0, 5.0)) {
      cerr << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
      cerr << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

      if (ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude())) {
        cerr << "Sample = " << ugm2.Sample() << endl;
        cerr << "Line = " << ugm2.Line() << endl << endl;
      }
    }

    cout << "For (23040, 5.0) ..." << endl;
    if (ugm2.SetImage(23040, 5.0)) {
      cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

      if (ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude())) {
        cout << "Sample = " << ugm2.Sample() << endl;
        cout << "Line = " << ugm2.Line() << endl << endl;
      }
    }

    cout << "For (2.0, 11520) ..." << endl;
    if (ugm2.SetImage(2.0, 11520)) {
      cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

      if (ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude())) {
        cout << "Sample = " << ugm2.Sample() << endl;
        cout << "Line = " << ugm2.Line() << endl << endl;
      }
    }

    cout << "For (23040, 11520) ..." << endl;
    if (ugm2.SetImage(23040, 11520)) {
      cout << "Universal Latitude = " << ugm2.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm2.UniversalLongitude() << endl;

      if (ugm2.SetUniversalGround(ugm2.UniversalLatitude(), ugm2.UniversalLongitude())) {
        cout << "Sample = " << ugm2.Sample() << endl;
        cout << "Line = " << ugm2.Line() << endl << endl;
      }
    }

    cout << "  Testing Camera Model and Projection..." << endl;
    Isis::Pvl p3("$mgs/testData/m0402852.cub");
    Isis::UniversalGroundMap ugm3(p3);
    cout << "Is Projection? = " << ugm3.HasProjection() << endl << endl;

    // Test all four corners to make sure the conversions are right
    cout << "For (1.0, 5.0) ..." << endl;
    if (ugm3.SetImage(1.0, 5.0)) {
      cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

      if (ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude())) {
        cout << "Sample = " << ugm3.Sample() << endl;
        cout << "Line = " << ugm3.Line() << endl << endl;
      }
    }

    cout << "For (1640, 20.0) ..." << endl;
    if (ugm3.SetImage(1640, 20.0)) {
      cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

      if (ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude())) {
        cout << "Sample = " << ugm3.Sample() << endl;
        cout << "Line = " << ugm3.Line() << endl << endl;
      }
    }

    cout << "For (30.0, 415) ..." << endl;
    if (ugm3.SetImage(30.0, 415)) {
      cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

      if (ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude())) {
        cout << "Sample = " << ugm3.Sample() << endl;
        cout << "Line = " << ugm3.Line() << endl << endl;
      }
    }

    cout << "For (1700, 245) ..." << endl;
    if (ugm3.SetImage(1700, 245)) {
      cout << "Universal Latitude = " << ugm3.UniversalLatitude() << endl;
      cout << "Universal Longitude = " << ugm3.UniversalLongitude() << endl;

      if (ugm3.SetUniversalGround(ugm3.UniversalLatitude(), ugm3.UniversalLongitude())) {
        cout << "Sample = " << ugm3.Sample() << endl;
        cout << "Line = " << ugm3.Line() << endl << endl;
      }
    }
  }
  catch (Isis::IException &e) {
    e.print();
  }
}

