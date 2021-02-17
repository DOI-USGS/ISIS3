/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <iomanip>
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
    alg += PvlKeyword("Name", "AdaptiveGruen");
    alg += PvlKeyword("Tolerance", "0.01");
    alg += PvlKeyword("AffineTranslationTolerance", "0.15");
    alg += PvlKeyword("AffineScaleTolerance", "0.15");
    alg += PvlKeyword("MaximumIterations", "30");

    PvlGroup pchip("PatternChip");
    pchip += PvlKeyword("Samples", "15");
    pchip += PvlKeyword("Lines", "15");

    PvlGroup schip("SearchChip");
    schip += PvlKeyword("Samples", "30");
    schip += PvlKeyword("Lines", "30");

    PvlObject o("AutoRegistration");
    o.addGroup(alg);
    o.addGroup(pchip);
    o.addGroup(schip);

    Pvl pvl;
    pvl.addObject(o);
    std::cout << pvl << std::endl;

    AutoReg *ar = AutoRegFactory::Create(pvl);

    Cube p;
    p.open("$ISISTESTDATA/isis/src/messenger/unitTestData/EW0131770376G.equi.cub");

    Cube s;
    s.open("$ISISTESTDATA/isis/src/messenger/unitTestData/EW0131770381F.equi.cub");

    ar->SearchChip()->TackCube(512.0, 512.0);
    ar->SearchChip()->Load(s);
    ar->PatternChip()->TackCube(512.0, 512.0);
    ar->PatternChip()->Load(p);

    std::cout << "Register = " << ar->Register() << std::endl;
    std::cout << "Goodness = " << std::setprecision(3)
              << ar->GoodnessOfFit() << std::endl;
    std::cout << "Position = " << std::setprecision(6)
              << ar->CubeSample() << " " << ar->CubeLine() << std::endl;


#if defined(FULL_DISCLOSURE)
    Pvl pstat = ar->RegistrationStatistics();
    std::cout << "\n\n" << pstat << std::endl;
#endif


  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
