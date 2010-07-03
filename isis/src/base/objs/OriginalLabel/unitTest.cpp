/**
 * Unit Test for OriginalLabel class.
 */


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
  p.AddGroup(g);

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
