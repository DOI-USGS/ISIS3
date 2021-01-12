#include <iostream>
#include "FileName.h"
#include "Pvl.h"
#include "ObservationNumber.h"
#include "Cube.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  FileName file("$lo/testData/3133_h1.cub");
  Pvl p1(file.expanded());

  cout << ObservationNumber::Compose(p1) << endl;

  return (0);
}
