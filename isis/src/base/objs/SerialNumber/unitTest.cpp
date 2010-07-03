#include <iostream>
#include "Filename.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "ObservationNumber.h"
#include "Cube.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  Isis::PvlGroup g("Instrument");
  g += Isis::PvlKeyword("SpacecraftName", "MySpacecraft");
  g += Isis::PvlKeyword("InstrumentId", "MyInstrumentId");
  g += Isis::PvlKeyword("SpacecraftClockCount", "987654321");

  Isis::PvlObject o("IsisCube");
  o.AddGroup(g);

  Isis::Pvl p;
  p.AddObject(o);

  std::cout << Isis::SerialNumber::Compose(p) << std::endl;

  p.FindGroup("Instrument",Isis::Pvl::Traverse).DeleteKeyword("InstrumentId");
  std::cout << Isis::SerialNumber::Compose(p) << std::endl;

  Isis::Cube cube;
  cube.Open("$base/testData/isisTruth.cub");
  std::cout << Isis::SerialNumber::Compose(cube, true) << std::endl;

  Isis::Filename file("$lo/testData/3133_h1.cub");
  Isis::Pvl p1(file.Expanded());

  std::cout << Isis::SerialNumber::Compose(p1) << std::endl;

  std::cout << std::endl << "Testing ObservationKeys" << std::endl;

  std::cout << Isis::ObservationNumber::Compose(p1) << std::endl;
  return (0);
}
