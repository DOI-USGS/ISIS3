#include "SessionLog.h"
#include "Application.h"
#include "Preference.h"
#include "iException.h"
#include "Pvl.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  Isis::PvlGroup &g = Isis::Preference::Preferences().FindGroup("SessionLog");
  g["TerminalOutput"] = "On";

  try {
    Isis::PvlGroup results("Results");;
    results.AddComment("// This is an example of the results group");
    results += Isis::PvlKeyword("Average",13.5,"Meters");
    results[0].AddComment("// Average size of a rock");

    Isis::Pvl error;
    Isis::PvlGroup temp("Error");
    temp += Isis::PvlKeyword("Program","ratio");
    temp += Isis::PvlKeyword("Class","I/O ERROR");
    temp += Isis::PvlKeyword("Status",-1);
    temp += Isis::PvlKeyword("Message","Unable to open file");
    temp += Isis::PvlKeyword("File","unitTest.cpp");
    temp += Isis::PvlKeyword("Line",501);
    error.AddGroup(temp);

    char *s_argv[] = {"unitTest","num=a", "den=b", "to=bogus"};
    int s_argc(4);
    try {
      Isis::Application app(s_argc,s_argv);
      Isis::SessionLog &log = Isis::SessionLog::TheLog(true);
      log.AddResults(results);
      std::cout << log << std::endl;
    } 
    catch (Isis::iException &e) {
      e.Report();
    }

    try {
      Isis::Application app(s_argc,s_argv);
      Isis::SessionLog &log = Isis::SessionLog::TheLog(true);
      log.AddResults(results);
      log.AddError(error);
      std::cout << log << std::endl;
    } 
    catch (Isis::iException &e) {
      e.Report();
    }

  }
  catch (Isis::iException &e) {
    e.Report();
  }

  return 0;
}
