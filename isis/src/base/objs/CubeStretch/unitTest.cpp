#include <iostream>

#include "CubeStretch.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Preference::Preferences(true);

  CubeStretch cubeStretch;
  std::cout << "Created a cubStretch without immediately failing.";
}
