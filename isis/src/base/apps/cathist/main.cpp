#include "Isis.h"

#include <sstream>

#include "FileName.h"
#include "IString.h"
#include "History.h"
#include "Pvl.h"   
#include "TextFile.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Get user entered file name & mode
  UserInterface &ui = Application::GetUserInterface();
  FileName fromfile(ui.GetFileName("FROM"));
  QString mode = ui.GetString("MODE");

  FileName tofile;
  bool append = false;
  if (ui.WasEntered("TO")) {
    tofile = FileName(ui.GetFileName("TO"));
    append = ui.GetBoolean("APPEND");
  }

  // Extract history from file
  History hist("IsisCube", fromfile.expanded());
  Pvl pvl = hist.ReturnHist();

  // Print full history
  if(mode == "FULL") {
    if(ui.IsInteractive()) {
      Application::GuiLog(pvl);
    }
    else if (ui.WasEntered("TO")) {
      if (append) {
        pvl.append(tofile.expanded());
      }
      else {
        pvl.write(tofile.expanded());
      }
    }
    else {
      cout << pvl << endl;
    }
  }

  // Print brief history in command line form
  else if(mode == "BRIEF") {
    TextFile * text = NULL;
    if (ui.WasEntered("TO")) {
      if (append) {
        text = new TextFile(tofile.expanded(),"append");
      }
      else {
        text = new TextFile(tofile.expanded(),"overwrite");
      }
    }
    for(int i = 0; i < pvl.objects(); ++i) {
      QString all = pvl.object(i).name() + " ";
      PvlGroup user = pvl.object(i).findGroup("UserParameters");
      for(int j = 0; j < user.keywords(); ++j) {
        ostringstream os;
        os << user[j];
        QString temp = os.str().c_str();
        int index = temp.indexOf("=");
        QString temp1(temp.mid(0, index - 1));
        QString temp2 = temp.mid(index + 2);
        all += temp1.toLower() + "=" + temp2 + " ";
      }
      if(ui.IsInteractive()) {
        Application::GuiLog(all);
      }
      else if (ui.WasEntered("TO")) {
        text->PutLine(all);
      }
      else {
        cout << all << endl;
      }
    }
    if (text) {
      text->Close();
      delete text;
      text = NULL;
    }
  }
}
