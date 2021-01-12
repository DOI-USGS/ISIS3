#include <iostream>
#include <cstdlib>

#include "AutoReg.h"
#include "Gruen.h"
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
    alg += PvlKeyword("Name", "Gruen");
    alg += PvlKeyword("Tolerance", toString(100.0));
    alg += PvlKeyword("AffineTranslationTolerance", toString(0.15));
    alg += PvlKeyword("AffineScaleTolerance", toString(0.3));
    alg += PvlKeyword("MaximumIterations", toString(30));

    PvlGroup pchip("PatternChip");
    pchip += PvlKeyword("Samples", toString(19));
    pchip += PvlKeyword("Lines", toString(19));

    PvlGroup schip("SearchChip");
    schip += PvlKeyword("Samples", toString(25));
    schip += PvlKeyword("Lines", toString(25));

    PvlObject o("AutoRegistration");
    o.addGroup(alg);
    o.addGroup(pchip);
    o.addGroup(schip);

    Pvl pvl;
    pvl.addObject(o);
    std::cout << pvl << std::endl;

    Gruen gruen(pvl);

    Cube p;
    p.open("$messenger/testData/EW0131770376G.equi.cub");

    Cube s;
    s.open("$messenger/testData/EW0131770381F.equi.cub");

    gruen.SearchChip()->TackCube(512.0, 512.0);
    gruen.SearchChip()->Load(s);
    gruen.PatternChip()->TackCube(512.0, 512.0);
    gruen.PatternChip()->Load(p);

    std::cout << "Register = " << gruen.Register() << std::endl;
    std::cout << "Position = " << gruen.CubeSample() << " " <<
              gruen.CubeLine() << std::endl;


#if defined(FULL_DISCLOSURE)
    Pvl pstat = gruen.RegistrationStatistics();
    std::cout << "\n\n" << pstat << std::endl;
#endif


  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
