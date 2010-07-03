#include <iostream>
#include "Preference.h"

#include "iException.h"

using namespace std;
using namespace Isis;

int main (void) {
  Isis::Preference::Preferences(true);

// Load up the error stack 

  iException::Message(iException::None,
                      "Testing unknown (none) errors",_FILEINFO_);
  iException::Message(iException::User,
                      "Testing user errors",_FILEINFO_);
  iException::Message(iException::Programmer,
                      "Testing programmer errors",_FILEINFO_);
  iException::Message(iException::Pvl,
                      "Testing PVL errors",_FILEINFO_);
  iException::Message(iException::Io,
                      "Testing I/O errors",_FILEINFO_);
  iException::Message(iException::Camera,
                      "Testing camera errors",_FILEINFO_);
  iException::Message(iException::Projection,
                      "Testing projection errors",_FILEINFO_);
  iException::Message(iException::Parse,
                      "Testing parse errors",_FILEINFO_);
  iException::Message(iException::Spice,
                      "Testing spice errors",_FILEINFO_);
  iException::Message(iException::System,
                      "Testing system errors",_FILEINFO_);
  iException &e =  iException::Message(iException::Math,
                                        "Testing math errors",_FILEINFO_);

// Only have to report one because we have generated a large stack
// of errors

  cout << "TEST CASES FOR ERROR OBJECTS" << endl;
  cout << "----------------------------" << endl;

  e.Report (false);

  e = iException::Message(iException::Cancel,"",_FILEINFO_);
  cout << "Testing cancel option ..." << endl;
  e.Report (false);

  return 0;
}
