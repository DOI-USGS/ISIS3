/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>

#include "Camera.h"
#include "Cube.h"
#include "FileName.h"
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
    Cube c("$ISISTESTDATA/isis/src/viking/unitTestData/f348b26.cub", "r");
    UniversalGroundMap ugm(c);
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
    Cube c2("$base/dems/molaMarsPlanetaryRadius0001.cub", "r");
    UniversalGroundMap ugm2(c2);
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
    Cube c3("$ISISTESTDATA/isis/src/mgs/unitTestData/m0402852.cub", "r");
    UniversalGroundMap ugm3(c3);
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

