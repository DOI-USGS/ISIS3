#include <iostream>
#include "FileName.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "ObservationNumber.h"
#include "Cube.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  PvlGroup g("Instrument");
  g += PvlKeyword("SpacecraftName", "MySpacecraft");
  g += PvlKeyword("InstrumentId", "MyInstrumentId");
  g += PvlKeyword("SpacecraftClockCount", "987654321");

  PvlObject o("IsisCube");
  o.AddGroup(g);

  Pvl p;
  p.AddObject(o);

  cout << SerialNumber::Compose(p) << endl;

  p.FindGroup("Instrument", Pvl::Traverse).DeleteKeyword("InstrumentId");
  cout << SerialNumber::Compose(p) << endl;

  Cube cube;
  cube.open("$base/testData/isisTruth.cub");
  cout << SerialNumber::Compose(cube, true) << endl;

  FileName file("$lo/testData/3133_h1.cub");
  Pvl p1(file.expanded());

  cout << SerialNumber::Compose(p1) << endl;

  cout << endl << "Testing ObservationKeys" << endl;

  cout << ObservationNumber::Compose(p1) << endl;
  return (0);
}
