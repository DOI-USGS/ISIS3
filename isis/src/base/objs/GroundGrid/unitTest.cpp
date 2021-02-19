/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"

#include <iostream>

#include "Angle.h"
#include "Cube.h"
#include "GroundGrid.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Progress.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "UniversalGroundMap.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  /*
    The output of this class directly correlates to the output of the "grid"
    application when mode=ground - if that application is correct, then this
    object is correct and vice versa.
   */
  Cube someCube;

  cout << "Reading cube..." << endl;
  someCube.open("$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.cub");

  cout << "Create universal ground map..." << endl;
  UniversalGroundMap gmap(someCube);

  cout << "Create grid..." << endl;
  Progress progress;
  GroundGrid grid(&gmap, false, false, someCube.sampleCount(), someCube.lineCount());
  grid.SetGroundLimits(Latitude(28.572438078395002, Angle::Degrees),
                       Longitude(-133.284402721991682, Angle::Degrees),
                       Latitude(34.340453944831125, Angle::Degrees),
                       Longitude(-134.060950006448195, Angle::Degrees));

  grid.CreateGrid(Latitude(0, Angle::Degrees), Longitude(0, Angle::Degrees),
      Angle(0.2, Angle::Degrees), Angle(0.2, Angle::Degrees),
      &progress, Angle(0.1, Angle::Degrees), Angle(0.01, Angle::Degrees));

  cout << "\n\nGrid cutout: \n" << endl;

  for(int line = 0; line < someCube.lineCount() / 4; line++) {

    for(int i = (int)(someCube.sampleCount() * 3.0 / 7.0);
            i < (someCube.sampleCount() * 3.5 / 7.0); i++) {
      cout << grid.PixelOnGrid(i, line);
    }

    cout << endl;
  }

  cout << endl;

  grid.WalkBoundary();

  cout << "\n\nGrid cutout with boundary walk: \n" << endl;

  for(int line = 0; line < someCube.lineCount() / 4; line++) {

    for(int i = (int)(someCube.sampleCount() * 3.0 / 7.0);
            i < (someCube.sampleCount() * 3.5 / 7.0); i++) {
      cout << grid.PixelOnGrid(i, line);
    }

    cout << endl;
  }

  cout << endl;

  cout << "Error checking for no lat/lon range:\n";
  cout.flush();
  try {
    Cube incompleteLabelsCube;
    incompleteLabelsCube.open("$ISISTESTDATA/isis/src/base/unitTestData/GroundGrid/unitTest.cub");
    UniversalGroundMap gmap(incompleteLabelsCube,
        UniversalGroundMap::ProjectionFirst);
    GroundGrid tmp(&gmap, false, false, someCube.sampleCount(), someCube.lineCount());
    Longitude invalidLon;
    Latitude invalidLat;
    tmp.SetGroundLimits(Latitude(28.572438078395002, Angle::Degrees),
                         invalidLon, invalidLat,
                         Longitude(-134.060950006448195, Angle::Degrees));
    tmp.CreateGrid(Latitude(0, Angle::Degrees), Longitude(0, Angle::Degrees),
        Angle(0.2, Angle::Degrees), Angle(0.2, Angle::Degrees),
        &progress, Angle(0.1, Angle::Degrees), Angle(0.01, Angle::Degrees));
  }
  catch(IException &e) {
    e.print();
  }
}
