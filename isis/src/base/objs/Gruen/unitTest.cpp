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

int main () {
  Isis::Preference::Preferences(true);

  try {
  PvlGroup alg("Algorithm");
  alg += PvlKeyword("Name","Gruen");
  alg += PvlKeyword("Tolerance",100.0);
  alg += PvlKeyword("AffineTranslationTolerance",0.2);
  alg += PvlKeyword("AffineScaleTolerance",0.7);
  alg += PvlKeyword("MaximumIterations",30);

  PvlGroup pchip("PatternChip");
  pchip += PvlKeyword("Samples",15);
  pchip += PvlKeyword("Lines",15);

  PvlGroup schip("SearchChip");
  schip += PvlKeyword("Samples",30);
  schip += PvlKeyword("Lines",30);

  PvlObject o("AutoRegistration");
  o.AddGroup(alg);
  o.AddGroup(pchip);
  o.AddGroup(schip);

  Pvl pvl;
  pvl.AddObject(o);
  std::cout << pvl << std::endl;

  Gruen gruen(pvl);

  Cube p;
  p.Open("$messenger/testData/EW0131770376G.equi.cub");

  Cube s;
  s.Open("$messenger/testData/EW0131770381F.equi.cub");

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
  catch (iException &e) {
    e.Report();
  }

  return 0;
}
