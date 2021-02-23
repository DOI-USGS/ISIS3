/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <fstream>
#include "Pvl.h"
#include "OriginalLabel.h"
#include "Preference.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  Isis::Pvl p;

  Isis::PvlGroup g("Test");
  g += Isis::PvlKeyword("Keyword", "Value");
  p.addGroup(g);

  cout << p << endl;
  Isis::OriginalLabel ol(p);

  ol.Write("olTemp");

  Isis::OriginalLabel ol2("olTemp");

  Isis::Pvl test = ol2.ReturnLabels();

  cout << endl;
  cout << test << endl;

  if(p == test) {
    cout << "Same pvls" << endl;
  }
  else {
    cout << "different pvls" << endl;
  }
  remove("olTemp");
}
