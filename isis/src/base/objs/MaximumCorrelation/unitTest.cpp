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

int main () {
  Isis::Preference::Preferences(true);

  try {
  PvlGroup alg("Algorithm");
  alg += PvlKeyword("Name","MaximumCorrelation");
  alg += PvlKeyword("Tolerance",0.1);
  alg += PvlKeyword("SubpixelAccuracy", "True");

  PvlGroup pchip("PatternChip");
  pchip += PvlKeyword("Samples",15);
  pchip += PvlKeyword("Lines",15);
  pchip += PvlKeyword("Sampling",50);
  pchip += PvlKeyword("ValidPercent", 10);

  PvlGroup schip("SearchChip");
  schip += PvlKeyword("Samples",35);
  schip += PvlKeyword("Lines",35);

  PvlObject o("AutoRegistration");
  o.AddGroup(alg);
  o.AddGroup(pchip);
  o.AddGroup(schip);

  Pvl pvl;
  pvl.AddObject(o);
  std::cout << pvl << std::endl;

  AutoReg *ar = AutoRegFactory::Create(pvl);

  Cube c;
  c.Open("$mgs/testData/ab102401.cub");

  ar->SearchChip()->TackCube(125.0,50.0);
  ar->SearchChip()->Load(c);
  ar->PatternChip()->TackCube(120.0,45.0);
  ar->PatternChip()->Load(c);

  std::cout << "Register = " << ar->Register() << std::endl;
  std::cout << "Position = " << ar->CubeSample() << " " << 
                                ar->CubeLine() << std::endl;
  }
  catch (iException &e) {
    e.Report();
  }

  return 0;
}
