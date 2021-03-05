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
#include "TileManager.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  QString fname = "IsisTileUnitTest";
  const int ns = 254;
  const int nl = 300;
  const int nb = 2;

  // Allocate a cube
  Cube *cube = new Cube;
  try {
    cube->setDimensions(ns, nl, nb);
    cube->create(fname);
  }
  catch(IException &e) {
    delete cube;
    e.print();
  }

  // Create a tile buffer for the cube with default size (128,128)
  TileManager tile(*cube);
  cout << "tiles = " << tile.Tiles() << endl;

  // Get each tile and output the sample, line and band of the upper left corner
  cout << "Coordinates of upper left pixel in each 128 x 128 tile" << endl;
  int tileNum = 1;
  for(tile.begin(); !tile.end(); tile++) {
    cout << "  Corner of tile " << tileNum++ << " is: ("
         << tile.Sample(0) << ", "
         << tile.Line(0) << ", "
         << tile.Band() << ")" << endl;
  }

  // Create a tile buffer for the cube with (91,113)
  TileManager tile2(*cube, 91, 113);

  // Get each tile and output the sample, line and band of the upper left corner
  cout << "Coordinates of upper left pixel in each 91 x 113 tile" << endl;
  tileNum = 1;
  for(tile2.begin(); !tile2.end(); tile2++) {
    cout << "  Corner of tile " << tileNum++ << " is: ("
         << tile2.Sample() << ", "
         << tile2.Line() << ", "
         << tile2.Band() << ")" << endl;
  }

  cout << "Coordinates of specific tiles in specific bands" << endl;
  tile.SetTile(1, 1);
  cout << "  Corner of tile 1 band 1 is: ("
       << tile.Sample() << ", "
       << tile.Line() << ", "
       << tile.Band() << ")" << endl;
  tile.SetTile(1, 2);
  cout << "  Corner of tile 1 band 2 is: ("
       << tile.Sample() << ", "
       << tile.Line() << ", "
       << tile.Band() << ")" << endl;
  tile2.SetTile(6, 1);
  cout << "  Corner of tile 6 band 1 is: ("
       << tile2.Sample() << ", "
       << tile2.Line() << ", "
       << tile2.Band() << ")" << endl;

  cube->close(true);
}
