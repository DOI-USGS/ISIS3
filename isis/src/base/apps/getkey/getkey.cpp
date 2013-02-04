#define GUIHELPERS

#include "Isis.h"

#include <QString>
#include <sstream>
#include "Process.h"
#include "Pvl.h"
#include "SessionLog.h"

using namespace std;
using namespace Isis;

//helper button functins in the code
void helperButtonLog();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["helperButtonLog"] = (void *) helperButtonLog;
  return helper;
}

void IsisMain() {
  // Set Preferences to always turn off Terminal Output
  PvlGroup &grp = Isis::Preference::Preferences().FindGroup("SessionLog", Isis::Pvl::Traverse);
  grp["TerminalOutput"].SetValue("Off");

  // Use a regular Process
  Process p;

  // Get the input file from the user interface
  UserInterface &ui = Application::GetUserInterface();
  QString labelFile = ui.GetFileName("FROM");

  // Open the file ... it must be a label-type file
  Pvl lab;
  lab.Read(labelFile);
  bool recursive = ui.GetBoolean("RECURSIVE");

  // Set up the requested object
  PvlKeyword key;
  if(ui.WasEntered("OBJNAME")) {
    QString obj = ui.GetString("OBJNAME");

    // Get the keyword from the entered group
    if(ui.WasEntered("GRPNAME")) {
      PvlObject object = lab.FindObject(obj, Pvl::Traverse);
      QString grp = ui.GetString("GRPNAME");
      key = object.FindGroup(grp, Pvl::Traverse)[ui.GetString("KEYWORD")];
    }
    // Find the keyword in the object
    else {
      if(recursive) {
        key = lab.FindObject(obj, Pvl::Traverse).FindKeyword(ui.GetString("KEYWORD"), Pvl::Traverse);
      }
      else {
        key = lab.FindObject(obj, Pvl::Traverse)[ui.GetString("KEYWORD")];
      }
    }
  }

  // Set up the requested group
  else if(ui.WasEntered("GRPNAME")) {
    QString grp = ui.GetString("GRPNAME");
    key = lab.FindGroup(grp, Pvl::Traverse)[ui.GetString("KEYWORD")];
  }

  // Find the keyword in the label, outside of any object or group
  else {
    if(recursive) {
      key = lab.FindKeyword(ui.GetString("KEYWORD"), Pvl::Traverse);
    }
    else {
      key = lab[ui.GetString("KEYWORD")];
    }
  }

  QString value;
  if(ui.WasEntered("KEYINDEX")) {
    int i = ui.GetInteger("KEYINDEX");

    // Make sure they requested a value inside the range of the list
    if(key.Size() < i) {
      QString msg = "The value entered for [KEYINDEX] is out of the array ";
      msg += "bounds for the keyword [" + ui.GetString("KEYWORD") + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    // Get the keyword value
    else value = key[i-1];
  }
  else {
    // Push the whole list into a IString and clean it up before returning it
    if(key.Size() > 1) {
      ostringstream os;
      os << key;
      QString temp = os.str().c_str();
      value = temp.mid(temp.indexOf("("), temp.indexOf(")" - temp.indexOf("("))).simplified();
    }
    // Just get the keyword value since it isnt a list
    else value = (QString)key;
  }

  if(ui.GetBoolean("UPPER")) value = value.toUpper();

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword("From", labelFile);
  results += PvlKeyword(ui.GetString("KEYWORD"), value);
  if(ui.IsInteractive()) {
    Application::GuiLog(results);
  }
  else {
    cout << value << endl;
  }
  // Write the results to the log but not the terminal
  SessionLog::TheLog().AddResults(results);
}

//Helper function to output the input file to log.
void helperButtonLog() {
  UserInterface &ui = Application::GetUserInterface();
  QString file(ui.GetFileName("FROM"));
  Pvl p;
  p.Read(file);
  Application::GuiLog(p);
}
//...........end of helper function LogMap ........
