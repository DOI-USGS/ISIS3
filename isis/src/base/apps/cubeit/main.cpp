#define GUIHELPERS
#include "Isis.h"

#include "Application.h"
#include "cubeit.h"
#include "TextFile.h"

using namespace Isis;
using namespace std;

//helper button function in the code
void helperButtonLog();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["helperButtonLog"] = (void *) helperButtonLog;
  return helper;
}


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    cubeit(ui, &appLog);
  }
  catch (...) {
    for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
      Application::Log(*grpIt);
    }
    throw;
  }

  for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
    Application::Log(*grpIt);
  }
}

//Helper function to output the input file to log.
void helperButtonLog() {
  UserInterface &ui = Application::GetUserInterface();
  QString file(ui.GetFileName("FROMLIST"));
  TextFile text(file);
  QString line;
  for(int i = 0; i < text.LineCount(); i++) {
    text.GetLine(line);
    Application::GuiLog(line);
  }
}
