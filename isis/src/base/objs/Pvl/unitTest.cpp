#include "Pvl.h"
#include "IException.h"
#include "Preference.h"

#include <iostream>
#include <sstream>

using namespace Isis;
using namespace std;

int main() {
  Preference::Preferences(true);

  Pvl p;
  p += PvlKeyword("LongKeyword", "This is a very long keyword value which "
                  "was causing some problems when the Pvl was output."
                  " The fist couple of lines looked good, but after that "
                  "things went south. Some lines get nothing, others get"
                  " bad indenting, most were too short");

  cout << "p: " << p << "\n";
  Pvl &ref = p;
  Pvl copy(p);
  copy.DeleteKeyword("LongKeyword");
  cout << "copy deleted a keyword...\np: " << p << "\n\ncopy: " << copy << "\n\n\n";

  PvlGroup g("Test");
  g += PvlKeyword("Keyword", "Value");
  p.AddGroup(g);

  p.SetTerminator("");
  ref.Write("tmp.unitTest");
  p.Append("tmp.unitTest");

  Pvl p2;
  p2.Read("tmp.unitTest");
  cout << p2 << endl << endl;

  Pvl p3;
  p3.Read("unitTest.pvl");
  cout << p3 << endl << endl;

  stringstream os;
  os << "temp = (a,b,c)";

  Pvl p4;
  os >> p4;
  cout << p4 << endl << endl;
  remove("tmp.unitTest");

  try {
    Pvl p5;
    p5.Read("unitTest2.pvl");
    cout << p5 << endl << endl;
  }
  catch(IException &e) {
    cout.flush();

    // make this error work regardless of directory...
    string errors = e.toString();

    while(errors.find("/") != string::npos) {
      int pos = errors.find("/");

      if(errors.find("/", pos + 1) < errors.find("]")) {
        errors = errors.substr(0, pos + 1) +
                 errors.substr(errors.find("/", pos + 1) + 1);
      }
      else {
        errors = errors.substr(0, pos - 1) + errors.substr(pos + 1);
      }
    }

    cout << errors;
  }

  cout << endl << endl;

  try {
    Pvl p6;
    p6.Read("unitTest3.pvl");
    cout << p6 << endl << endl;
  }
  catch(IException &e) {
    cout.flush();

    // make this error work regardless of directory...
    string errors = e.toString();

    while(errors.find("/") != string::npos) {
      int pos = errors.find("/");

      if(errors.find("/", pos + 1) < errors.find("]")) {
        errors = errors.substr(0, pos + 1) +
                 errors.substr(errors.find("/", pos + 1) + 1);
      }
      else {
        errors = errors.substr(0, pos - 1) + errors.substr(pos + 1);
      }
    }

    cout << errors;
  }

  cout << endl << endl;

  cout << "Testing MESSENGER labels with data at bottom..." << endl << endl;
  try {
    Pvl p7;
    p7.Read("unitTest4.pvl");
    cout << p7 << endl << endl;
  }
  catch(IException &e) {
    cout.flush();

    // make this error work regardless of directory...
    string errors = e.toString();

    while(errors.find("/") != string::npos) {
      int pos = errors.find("/");

      if(errors.find("/", pos + 1) < errors.find("]")) {
        errors = errors.substr(0, pos + 1) +
                 errors.substr(errors.find("/", pos + 1) + 1);
      }
      else {
        errors = errors.substr(0, pos - 1) + errors.substr(pos + 1);
      }
    }

    cout << errors;
  }

  // Validate a PVL
  Pvl pvlTmpl("cnetstatsTest.def");
  cout << "\n\n***Template PVL**\n" << pvlTmpl << endl;

  Pvl pvlUser("pointdef.def");
  cout << "\n\n***Test PVL**\n" << pvlUser << endl;

  Pvl pvlResults;
  pvlTmpl.ValidatePvl(pvlUser, pvlResults);
  cout << "\n\n**Result PVL**\n" << pvlResults << endl;
}
