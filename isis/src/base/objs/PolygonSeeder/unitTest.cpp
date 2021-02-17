/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>

#include "IException.h"
#include "PolygonSeeder.h"
#include "PolygonSeederFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main() {
  Isis::Preference::Preferences(true);

  try {
    PvlGroup alg("PolygonSeederAlgorithm");
    alg += PvlKeyword("Name", "Grid");
    alg += PvlKeyword("MinimumThickness", "0.5");
    alg += PvlKeyword("MinimumArea", "10");
    alg += PvlKeyword("XSpacing", "11");
    alg += PvlKeyword("YSpacing", "11");

    PvlObject o("AutoSeed");
    o.addGroup(alg);

    Pvl pvl;
    pvl.addObject(o);
    std::cout << pvl << std::endl << std::endl;

    PolygonSeeder *ps = PolygonSeederFactory::Create(pvl);

    Pvl p = ps->InvalidInput();
    cout << "Test invalidInput() ...\n" << p << "\n\n";

    std::cout << "Test to make sure Parse did it's job" << std::endl;
    std::cout << "MinimumThickness = " << ps->MinimumThickness() << std::endl;
    std::cout << "MinimumArea = " << ps->MinimumArea() << std::endl;

    std::cout << "No reason to test GridPolygonSeeder, so we're done" << std::endl;

  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
