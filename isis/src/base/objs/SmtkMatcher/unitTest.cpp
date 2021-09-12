#include <iostream>
#include <cstdlib>

#include "Cube.h"
#include "Preference.h"
#include "SmtkMatcher.h"

using namespace Isis;

int main() {
  Preference::Preferences(true);
  NaifContextLifecycle naif_lifecycle;
  auto naif = NaifContext::acquire();

  try {
    Cube rhImage;
    rhImage.open("$mariner10/testData/0027399_clean_equi.cub");

    Cube lhImage;
    lhImage.open("$mariner10/testData/0166613_clean_equi.cub");

    SmtkMatcher matcher("mar10.def", &lhImage, &rhImage);
    SmtkPoint spnt = matcher.Register(naif, Coordinate(272.813,208.293));

    std::cout << "IsValid       = " << spnt.isValid() << std::endl;
    std::cout << "Registered    = " << spnt.isRegistered() << std::endl;
    std::cout << "GoodnessOfFit = " << spnt.GoodnessOfFit() << std::endl;

  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
