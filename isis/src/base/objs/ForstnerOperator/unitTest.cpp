/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>
#include "InterestOperator.h"
#include "InterestOperatorFactory.h"
#include "ForstnerOperator.h"
#include "Chip.h"
#include "Cube.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Preference.h"

using namespace Isis;

int main() {
  try {
    Isis::Preference::Preferences(true);
    PvlGroup op("Operator");
    op += PvlKeyword("Name", "Forstner");
    op += PvlKeyword("DeltaLine", "100");
    op += PvlKeyword("DeltaSamp", "100");
    op += PvlKeyword("Samples", "15");
    op += PvlKeyword("Lines", "15");
    op += PvlKeyword("MinimumInterest", "0.0");

    PvlGroup opv("ValidMeasure"); 
    opv += PvlKeyword("MinDN", "0.0");
    opv += PvlKeyword("MaxDN", "1.0");
    opv += PvlKeyword("MinEmission", "15.0");
    opv += PvlKeyword("MaxEmission", "25.0");
    opv += PvlKeyword("MinIncidence", "0.0");
    opv += PvlKeyword("MaxIncidence", "135.0");

    PvlObject o("InterestOperator");
    o.addGroup(op);
    o.addGroup(opv);

    Pvl pvl;
    pvl.addObject(o);
    std::cout << pvl << std::endl;

    InterestOperator *iop = InterestOperatorFactory::Create(pvl);

    Cube c;
    c.open("$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.cub");

    //iop->Operate(c, 100, 350);
    UniversalGroundMap univGrndMap(c);
    iop->Operate(c,  univGrndMap, 100, 350);

    std::cout << "Sample: " << iop->CubeSample() << std::endl
              << "Line : " << iop->CubeLine() << std::endl
              << "Interest: " << iop->InterestAmount() << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
