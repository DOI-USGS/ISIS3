#include <string>
#include <iostream>
#include "Application.h"
#include "iException.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "Pvl.h"

void myFunct() {
  std::cout << "In myFunct" << std::endl;
}

void myError() {
  std::string msg = "testing an error";
  throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
}

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  std::cout << "Testing Isis::Application Class ... " << std::endl;
  std::cout << std::endl;
  Isis::Application a(argc, argv);

  Isis::PvlGroup acct = a.Accounting();
  a.Log(acct);
  std::cout << std::endl;

  Isis::PvlObject hist = a.History();
  std::cout << hist << std::endl;
  std::cout << std::endl;

  std::cout << "Can't test Gui methods" << std::endl;
  std::cout << "  GuiLog(Pvl &results)" << std::endl;
  std::cout << "  GuiLog(PvlGroup &results)" << std::endl;
  std::cout << "  GuiLog(std::string &results)" << std::endl;
  std::cout << "  GuiReportErrorLog(iException &e)" << std::endl;
  std::cout << std::endl;

  std::map<std::string, void *> helpers;
  helpers["helper"] = (void *) myFunct;
  a.RegisterGuiHelpers(helpers);
  if(a.GetGuiHelper("helper") == (void *) myFunct) {
    std::cout << "GuiHelpers ok" << std::endl;
  }
  else {
    std::cout << "GuiHelpers bad" << std::endl;
  }
  std::cout << std::endl;

  a.Exec(myFunct);
  std::cout << std::endl;

  a.Exec(myError);
  std::cout << std::endl;

  Isis::Pvl p("print.prt");
  std::cout << p << std::endl;

  remove("print.prt");
  return 0;
}
