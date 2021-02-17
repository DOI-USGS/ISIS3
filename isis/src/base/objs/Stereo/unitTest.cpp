/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>

#include "Camera.h"
#include "Cube.h"
#include "IException.h"
#include "Preference.h"
#include "Stereo.h"


using namespace std;
using namespace Isis;

/**
 * unitTest for Stereo class
 * 
 * @author 2012-02-29 Tracie Sucharski
 *  
 * @internal 
 * 
 */
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    cout << "UnitTest for Stereo" << endl;

    cout << setprecision(9);

    Cube leftCube;
    leftCube.open("$ISISTESTDATA/isis/src/mariner/unitTestData/0027399_clean_equi.cub");
    Cube rightCube;
    rightCube.open("$ISISTESTDATA/isis/src/mariner/unitTestData/0166613_clean_equi.cub");

    leftCube.camera()->SetImage(1054.19, 624.194);
    rightCube.camera()->SetImage(1052.19, 624.194);

    double radius, lat, lon, sepang, error;
    Stereo::elevation(*(leftCube.camera()), *(rightCube.camera()),
                      radius, lat, lon, sepang, error);

    cout << "Radius = " << radius << endl;
    cout << "Radius Error = " << error << endl;
    cout << "Separation Angle = " << sepang << endl;
    cout << "Latitude = " << lat << endl;
    cout << "Longitude = " << lon << endl;

    double x, y, z;
    Stereo::spherical(lat, lon, radius, x, y, z);
    cout << "Spherical to Rectangular conversion:" << endl;
    cout << "X = " << x << endl;
    cout << "Y = " << y << endl;
    cout << "Z = " << z << endl;

    double newLat, newLon, newRad;
    Stereo::rectangular(x, y, z, newLat, newLon, newRad);
    cout << "Rectangular to spherical conversion:" << endl;
    cout << "Latitude = " << newLat << endl;
    cout << "Longitude = " << newLon << endl;


  }
  catch (Isis::IException &e) {
    e.print();
  }
}

