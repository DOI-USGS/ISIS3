#include <iostream>
#include "FileName.h"
#include "Pvl.h"
#include "ObservationNumber.h"
#include "Cube.h"
#include "Preference.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  Isis::FileName file("$lo/testData/3133_h1.cub");
  Isis::Pvl p1(file.expanded());

  std::cout << Isis::ObservationNumber::Compose(p1) << std::endl;

  return (0);
}
