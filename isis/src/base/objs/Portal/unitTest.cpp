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

  QString fname = "$base/testData/isisTruth.cub";

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
