/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include "Preference.h"

using namespace std;

int main() {
  Isis::Preference::Preferences(true);

  // TODO actually test this
  cout << "Testing Isis::ImageExporter ..." << endl;
  cout << "Test deferred to the appTest for isis2std" << endl;

  return (0);
}
