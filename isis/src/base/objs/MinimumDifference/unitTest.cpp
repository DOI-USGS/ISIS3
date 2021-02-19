/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Chip.h"
#include "Cube.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  try {
    PvlGroup alg("Algorithm");
    alg += PvlKeyword("Name", "MinimumDifference");
    alg += PvlKeyword("Tolerance", "5.0");
    alg += PvlKeyword("SubpixelAccuracy", "True");

    PvlGroup pchip("PatternChip");
    pchip += PvlKeyword("Samples", "15");
    pchip += PvlKeyword("Lines", "15");
    pchip += PvlKeyword("Sampling", "25");
    pchip += PvlKeyword("ValidPercent", "10");

    PvlGroup schip("SearchChip");
    schip += PvlKeyword("Samples", "35");
    schip += PvlKeyword("Lines", "35");
    schip += PvlKeyword("Sampling", "50");

    PvlObject o("AutoRegistration");
    o.addGroup(alg);
    o.addGroup(pchip);
    o.addGroup(schip);

    Pvl pvl;
    pvl.addObject(o);
    std::cout << pvl << std::endl;

    AutoReg *ar = AutoRegFactory::Create(pvl);

    Cube c;
    c.open("$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.cub");

    ar->SearchChip()->TackCube(125.0, 50.0);
    ar->SearchChip()->Load(c);
    ar->PatternChip()->TackCube(120.0, 45.0);
    ar->PatternChip()->Load(c);

    std::cout << "Register = " << ar->Register() << std::endl;
    std::cout << "Position = " << ar->CubeSample() << " " <<
              ar->CubeLine() << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
