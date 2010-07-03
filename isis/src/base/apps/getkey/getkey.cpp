#define GUIHELPERS

#include "Isis.h"

#include <string>
#include <sstream>
#include "Process.h"
#include "Pvl.h"
#include "SessionLog.h"

using namespace std; 
using namespace Isis;

//helper button functins in the code
void helperButtonLog();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["helperButtonLog"] = (void*) helperButtonLog;
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
  string labelFile = ui.GetFilename("FROM");

  // Open the file ... it must be a label-type file
  Pvl lab;
  lab.Read(labelFile);
  bool recursive = ui.GetBoolean("RECURSIVE");

  // Set up the requested object
  PvlKeyword key;
  if (ui.WasEntered("OBJNAME")) {
    string obj = ui.GetString("OBJNAME");

    // Get the keyword from the entered group
    if (ui.WasEntered("GRPNAME")) {
      PvlObject object = lab.FindObject(obj,Pvl::Traverse);
      string grp = ui.GetString("GRPNAME");
      key = object.FindGroup(grp,Pvl::Traverse)[ui.GetString("KEYWORD")];
    }
    // Find the keyword in the object
    else {
      if(recursive){
        key = lab.FindObject(obj,Pvl::Traverse).FindKeyword(ui.GetString("KEYWORD"),Pvl::Traverse);
      } else {
        key = lab.FindObject(obj,Pvl::Traverse)[ui.GetString("KEYWORD")];
      }
    }
  }

  // Set up the requested group
  else if (ui.WasEntered("GRPNAME")) {
    string grp = ui.GetString("GRPNAME");
    key = lab.FindGroup(grp,Pvl::Traverse)[ui.GetString("KEYWORD")];
  }

  // Find the keyword in the label, outside of any object or group
  else {
    if(recursive){
        key = lab.FindKeyword(ui.GetString("KEYWORD"),Pvl::Traverse);
      } else {
        key = lab[ui.GetString("KEYWORD")];
      }
  }

  iString value;
  if (ui.WasEntered("KEYINDEX")) {
    int i = ui.GetInteger("KEYINDEX");

    // Make sure they requested a value inside the range of the list
    if (key.Size() < i) {
      string msg = "The value entered for [KEYINDEX] is out of the array ";
      msg += "bounds for the keyword [" + ui.GetString("KEYWORD") + "]";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    // Get the keyword value
    else value = key[i-1];
  }
  else {
    // Push the whole list into a iString and clean it up before returning it
    if (key.Size() > 1) {
      ostringstream os;
      os << key;
      iString temp = os.str();
      temp.Token("(");
      value = temp.Token(")");
      value = value.ConvertWhiteSpace();
      value = value.Compress();
    }
    // Just get the keyword value since it isnt a list
    else value = (string)key;
  }     

  if (ui.GetBoolean("UPPER")) value.UpCase();

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword ("From", labelFile);
  results += PvlKeyword (ui.GetString("KEYWORD"),value);
  if (ui.IsInteractive()) {
    Application::GuiLog(results);
  }
  else {
    cout << value << endl;
  }
  // Write the results to the log but not the terminal
  SessionLog::TheLog().AddResults(results);
}

//Helper function to output the input file to log.
void helperButtonLog () {
  UserInterface &ui = Application::GetUserInterface();
  string file(ui.GetFilename("FROM"));
  Pvl p;
  p.Read(file);
  Application::GuiLog(p);
}
//...........end of helper function LogMap ........
