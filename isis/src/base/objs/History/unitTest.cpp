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
