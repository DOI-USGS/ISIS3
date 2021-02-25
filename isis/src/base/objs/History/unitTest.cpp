/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"

#include <string>
#include <iostream>

#include "Pvl.h"
#include "History.h"
#include "Preference.h"

using namespace std;
void IsisMain() {
  Isis::Preference::Preferences(true);

  Isis::History h("Haha");;
  h.AddEntry();
  QString file = "unitTest.tttt";
  h.Write(file);

  Isis::History h2("Haha", file);
  Isis::PvlObject o = h2.ReturnHist();
  std::cout << o << std::endl;

  remove(file.toLatin1().data());
}
