#include "SessionLog.h"

#include <string.h>

#include "Application.h"
#include "Preference.h"
#include "IException.h"
#include "Pvl.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  PvlGroup &g = Preference::Preferences().FindGroup("SessionLog");
  g["TerminalOutput"] = "On";
  try {
    PvlGroup results("Results");;
    results.AddComment("// This is an example of the results group");
    results += PvlKeyword("Average", toString(13.5), "Meters");
    results[0].AddComment("// Average size of a rock");

    Pvl error;
    PvlGroup temp("Error");
    temp += PvlKeyword("Program", "ratio");
    temp += PvlKeyword("Class", "I/O ERROR");
    temp += PvlKeyword("Status", toString(-1));
    temp += PvlKeyword("Message", "Unable to open file");
    temp += PvlKeyword("File", "unitTest.cpp");
    temp += PvlKeyword("Line", toString(501));
    error.AddGroup(temp);
    char **s_argv;
    s_argv = new char*[10];
    s_argv[0] = new char[32];
    strncpy(s_argv[0], "unitTest", strlen("unitTest") + 1);
    s_argv[1] = new char[32];
    strncpy(s_argv[1], "num=a", strlen("num=a") + 1);
    s_argv[2] = new char[32];
    strncpy(s_argv[2], "den=b", strlen("den=b") + 1);
    s_argv[3] = new char[32];
    strncpy(s_argv[3], "to=bogus", strlen("to=bogus") + 1);
    s_argv[4] = 0;

    int s_argc = 4;
    try {
      Application app(s_argc, (char**)s_argv);
      SessionLog &log = SessionLog::TheLog(true);
      log.AddResults(results);
      std::cout << log << std::endl;
    }
    catch(IException &e) {
      e.print();
    }

    try {
      Application app(s_argc, (char**)s_argv);
      SessionLog &log = SessionLog::TheLog(true);
      log.AddResults(results);
      log.AddError(error);
      std::cout << log << std::endl;
    }
    catch(IException &e) {
      e.print();
    }

  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
