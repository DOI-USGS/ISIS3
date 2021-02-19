/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>

#include "Cube.h"
#include "Preference.h"
#include "SmtkMatcher.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  try {
    Cube rhImage;
    rhImage.open("$ISISTESTDATA/isis/src/mariner/unitTestData/0027399_clean_equi.cub");

    Cube lhImage;
    lhImage.open("$ISISTESTDATA/isis/src/mariner/unitTestData/0166613_clean_equi.cub");

    SmtkMatcher matcher("mar10.def", &lhImage, &rhImage);
    SmtkPoint spnt = matcher.Register(Coordinate(272.813,208.293));

    std::cout << "IsValid       = " << spnt.isValid() << std::endl;
    std::cout << "Registered    = " << spnt.isRegistered() << std::endl;
    std::cout << "GoodnessOfFit = " << spnt.GoodnessOfFit() << std::endl;

  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
