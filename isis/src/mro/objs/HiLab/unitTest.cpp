/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>

#include "HiLab.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  //create a dummy Isis::Progress object
  Cube cube;
  cube.open("$ISISTESTDATA/isis/src/mro/unitTestData/HiLab/red3Test.cub");

  cout << "Testing constructor ...\n";
  HiLab hiLab(&cube);
  cout << "Testing getCpmmNumber() ...\n";
  cout << "CpmmNumber " << hiLab.getCpmmNumber() << endl;
  cout << "Testing getChannel() ...\n";
  cout << "Channel "  << hiLab.getChannel() << endl;
  cout << "Testing getBin() ...\n";
  cout << "Bin " << hiLab.getBin() << endl;
  cout << "Testing getTdi() ...\n";
  cout << "Tdi " << hiLab.getTdi() << endl;
  cout << "Testing getCcd() ...\n";
  cout << "Ccd " << hiLab.getCcd() << endl;

  return 0;
}
