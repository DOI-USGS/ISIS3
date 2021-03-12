/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <iostream>
#include <stdio.h>

#include "Preference.h"
#include "Cube.h"
#include "BoxcarManager.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {

  Preference::Preferences(true);

  QString fname = "$ISISTESTDATA/isis/src/base/unitTestData/isisTruth.cub";
  Cube cube;
  cube.open(fname);

  //  Test 5x5 boxcar
  BoxcarManager box5x5(cube, 5, 5);
  cout << "Buffer (Boxcar) Size:  " <<
       box5x5.SampleDimension() << " " <<
       box5x5.LineDimension() << " " <<
       box5x5.BandDimension() << endl;
  cout << endl;

  for(box5x5.begin(); !box5x5.end(); box5x5++) {
    if(box5x5.Sample() <= 0) {
      cout << "  Coordinates of upper left corner of boxcar, sample, line, band is:  "
           << box5x5.Sample() << " "
           << box5x5.Line() << " "
           << box5x5.Band() << endl;
    }
  }
  cout << endl;

  //  Test 4x4 boxcar
  BoxcarManager box4x4(cube, 4, 4);
  cout << "Buffer (Boxcar) Size:  " <<
       box4x4.SampleDimension() << " " <<
       box4x4.LineDimension() << " " <<
       box4x4.BandDimension() << endl;
  cout << endl;

  for(box4x4.begin(); !box4x4.end(); box4x4++) {
    if(box4x4.Sample() <= 0) {
      cout << " Coordinates of upper left corner of boxcar,  sample, line, band is:  "
           << box4x4.Sample() << " "
           << box4x4.Line() << " "
           << box4x4.Band() << endl;
    }
  }
  cout << endl;

  cube.close();
}
