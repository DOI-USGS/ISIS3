/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <iostream>
#include <stdio.h>
#include "IException.h"
#include "Cube.h"
#include "Portal.h"
#include "Preference.h"


using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  QString fname = "$ISISTESTDATA/isis/src/base/unitTestData/isisTruth.cub";

  // Allocate a cube
  Isis::Cube *cube = new Isis::Cube;
  try {
    cube->open(fname);
  }
  catch(Isis::IException &e) {
    delete cube;
    e.print();
    exit(1);
  }

  // Create a portal buffer for the cube with a size of 1x1
  Isis::Portal portal(1, 1, cube->pixelType());

  // Get some portals and output the sample, line and band of the upper left corner
  cout << "Coordinates and value of upper left pixel in several portals:" << endl;
  portal.SetPosition(1, 1, 1);
  cube->read(portal);
  cout << "  Corner of portal 1 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(1, 1, 2);
  cube->read(portal);
  cout << "  Corner of portal 2 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(10, 10, 1);
  cube->read(portal);
  cout << "  Corner of portal 3 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(126, 126, 2);
  cube->read(portal);
  cout << "  Corner of portal 4 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(100, 101, 1);
  cube->read(portal);
  cout << "  Corner of portal 5 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(126, 1, 1);
  cube->read(portal);
  cout << "  Corner of portal 6 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(1, 126, 1);
  cube->read(portal);
  cout << "  Corner of portal 7 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetPosition(1, 1, 1);
  cube->read(portal);
  cout << "  Corner of portal 8 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetHotSpot(0, 0);
  portal.SetPosition(126, 126, 2);
  cube->read(portal);
  cout << "  Corner of portal 9 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  portal.SetHotSpot(13, 24);
  portal.SetPosition(126, 126, 2);
  cube->read(portal);
  cout << "  Corner of portal 10 is: ("
       << portal.Sample(0) << ", "
       << portal.Line(0) << ", "
       << portal.Band() << ")" << " = " << portal.DoubleBuffer()[0] << endl;

  cube->close();
}
