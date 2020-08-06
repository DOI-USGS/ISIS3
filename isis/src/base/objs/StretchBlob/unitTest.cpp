#include <iostream>

#include "StretchBlob.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Preference::Preferences(true);

  StretchBlob s;

  std::cout << "Creating a StretchBlob without immediate failure!" << std::endl; 

  return 0;
}
