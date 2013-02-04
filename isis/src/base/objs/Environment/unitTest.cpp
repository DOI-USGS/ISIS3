#include "Environment.h"

#include <cstring>

#include "IString.h"
#include "Preference.h"
#include "ProgramLauncher.h"


using namespace Isis;
using std::cerr;


int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  
  char * str = new char[256];
  strncpy(str, "USER=eclipse", 256);
  putenv(str);
  str = new char[256];
  strncpy(str, "HOST=wang", 256);
  putenv(str);
  
  cerr << "userName: " << Environment::userName() << "\n";
  cerr << "hostName: " << Environment::hostName() << "\n";
  
  QString cmd = "echo 'version:  isis" + Environment::isisVersion() + "' | cut -d . -f1";
  ProgramLauncher::RunSystemCommand(cmd);

  return 0;
}
