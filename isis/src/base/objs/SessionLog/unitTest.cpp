#include "SessionLog.h"

#include <string.h>

#include "Application.h"
#include "Preference.h"
#include "IException.h"
#include "Pvl.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  Isis::PvlGroup &g = Isis::Preference::Preferences().FindGroup("SessionLog");
  g["TerminalOutput"] = "On";
  try {
    Isis::PvlGroup results("Results");;
    results.AddComment("// This is an example of the results group");
    results += Isis::PvlKeyword("Average", 13.5, "Meters");
    results[0].AddComment("// Average size of a rock");

    Isis::Pvl error;
    Isis::PvlGroup temp("Error");
    temp += Isis::PvlKeyword("Program", "ratio");
    temp += Isis::PvlKeyword("Class", "I/O ERROR");
    temp += Isis::PvlKeyword("Status", -1);
    temp += Isis::PvlKeyword("Message", "Unable to open file");
    temp += Isis::PvlKeyword("File", "unitTest.cpp");
    temp += Isis::PvlKeyword("Line", 501);
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
      Isis::Application app(s_argc, (char**)s_argv);
      Isis::SessionLog &log = Isis::SessionLog::TheLog(true);
      log.AddResults(results);
      std::cout << log << std::endl;
    }
    catch(Isis::IException &e) {
      e.print();
    }

    try {
      Isis::Application app(s_argc, (char**)s_argv);
      Isis::SessionLog &log = Isis::SessionLog::TheLog(true);
      log.AddResults(results);
      log.AddError(error);
      std::cout << log << std::endl;
    }
    catch(Isis::IException &e) {
      e.print();
    }

  }
  catch(Isis::IException &e) {
    e.print();
  }

  return 0;
}
