#include "Isis.h"

#include <QString>

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  bool append = ui.GetBoolean("APPEND");
  Pvl input(ui.GetFileName("FROM"));
  Pvl output;

  // Check to see if output file exists
  FileName outFile = ui.GetFileName("TO");
  if(outFile.fileExists() && !append) {
    QString msg = "Output file [" + outFile.expanded() + "] already exists.";
    msg += " Append option set to False.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  int numErrors = 0;
  // Search for errors and add to output
  for(int i = 0; i < input.Objects(); i++) {
    PvlObject o = input.Object(i);
    if(o.HasGroup("Error")) {
      output.AddObject(o);
      numErrors++;
    }
  }
  PvlKeyword errors("TotalErrors", toString(numErrors));
  output.AddKeyword(errors);
  // write output to file
  if(!append) {
    output.Write(outFile.expanded());
  }
  else {
    output.Append(outFile.expanded());
  }
  cout << errors << endl;
}
