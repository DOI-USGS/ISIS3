#include "Isis.h"

#include <string>

using namespace std; 
using namespace Isis;

void IsisMain() {
  
  UserInterface &ui = Application::GetUserInterface();
  bool append = ui.GetBoolean("APPEND");
  Pvl input (ui.GetFilename("FROM"));
  Pvl output;

  // Check to see if output file exists
  Filename outFile = ui.GetFilename("TO");
  if (outFile.exists()&&!append) {
    string msg = "Output file [" + outFile.Expanded() + "] already exists.";
    msg += " Append option set to False.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  int numErrors = 0;
  // Search for errors and add to output
  for (int i =0; i < input.Objects(); i++) {
    PvlObject o = input.Object(i);
    if (o.HasGroup("Error")) {
      output.AddObject(o);
      numErrors++;
    }
  }
  PvlKeyword errors("TotalErrors",numErrors);
  output.AddKeyword(errors);
  // write output to file
  if (!append) {
    output.Write(outFile.Expanded());
  } else {
    output.Append(outFile.Expanded());
  }
  cout << errors << endl;
}
