#include <iostream>
#include <cstdlib>
#include "InterestOperator.h"
#include "InterestOperatorFactory.h"
#include "StandardDeviationOperator.h"
#include "Chip.h"
#include "Cube.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Preference.h"
#include "UniversalGroundMap.h"
#include "ControlNetValidMeasure.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);
  try {
    PvlGroup op("Operator");
    op += PvlKeyword("Name", "StandardDeviation");
    op += PvlKeyword("DeltaLine", 100);
    op += PvlKeyword("DeltaSamp", 100);
    op += PvlKeyword("Samples", 15);
    op += PvlKeyword("Lines", 15);
    op += PvlKeyword("MinDN", 0.0);
    op += PvlKeyword("MaxDN", 1.0);
    op += PvlKeyword("MinimumInterest", 0.01);
    op += PvlKeyword("MinEmission", 15.0);
    op += PvlKeyword("MaxEmission", 25.0);
    op += PvlKeyword("MinIncidence", 0.0);
    op += PvlKeyword("MaxIncidence", 135.0);
    //op += PvlKeyword("MinResolution", 100);
    //op += PvlKeyword("MaxResolution", 300);

    PvlObject o("InterestOperator");
    o.AddGroup(op);

    Pvl pvl;
    pvl.AddObject(o);
    std::cout << pvl << std::endl;

    InterestOperator *iop = InterestOperatorFactory::Create(pvl);

    Cube c;
    c.Open("$mgs/testData/ab102401.cub");

    UniversalGroundMap univGrndMap(c);
    iop->Operate(c,  univGrndMap, 100, 350);

    std::cout << "Sample: " << iop->CubeSample() << std::endl
              << "Line : " << iop->CubeLine() << std::endl
              << "Interest: " << iop->InterestAmount() << std::endl;
  }
  catch(iException &e) {
    e.Report();
  }



  return 0;
}
