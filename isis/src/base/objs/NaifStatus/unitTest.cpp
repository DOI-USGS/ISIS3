/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <SpiceUsr.h>

#include "IException.h"
#include "NaifStatus.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "Unit Test for NaifStatus" << std::endl;

  std::cout << "No Errors" << std::endl;
  NaifStatus::CheckErrors();

  std::cout << std::endl << "Empty String Error" << std::endl;
  try {
    SpiceChar *tmp = new SpiceChar[128];
    tmp[0] = '\0';
    erract_c("SET", (SpiceInt)0, tmp);
    NaifStatus::CheckErrors();
  }
  catch(IException &e) {
    e.print();
  }

}
