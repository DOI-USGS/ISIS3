#define GUIHELPERS

#include "Isis.h"

#include <fstream>

#include "Blob.h"
#include "Table.h"
#include "Filename.h"
#include "iString.h"
#include <sstream>

using namespace std;
using namespace Isis;

int pos = 0;
string previousFile = "";

void helperButtonGetBlobList();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["helperButtonGetBlobList"] = (void*) helperButtonGetBlobList;
  return helper;
}

void IsisMain(){
  UserInterface &ui = Application::GetUserInterface();
  Filename file = ui.GetFilename("FROM");
  string blobname = ui.GetString("NAME");
  string blobtype = ui.GetString("TYPE");
  Blob blob( blobname, blobtype, file.Expanded());
  Filename outfname = ui.GetFilename("TO");
  blob.Write(outfname.Expanded());
}

// Function to find the available blob names/types and put them into the GUI
void helperButtonGetBlobList() {
  string name, type;
  bool match = false;

  UserInterface &ui = Application::GetUserInterface();
  string currentFile = ui.GetFilename("FROM");
  const Pvl label (Filename(currentFile).Expanded());

  // Check to see if the "FILE" parameter has changed since last press
  if (currentFile != previousFile) {
    ui.Clear("NAME");
    ui.Clear("TYPE");
    pos = 0;
    previousFile = currentFile;
  }
  
  // Look for blobs
  int cnt = 0;
  while (!match) {   
    // If we've gone through all objects and found nothing, throw an exception
    if (cnt >= label.Objects()) {
      pos = 0;
      string msg = "Parameter [FROM] has no blobs.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    // When the end of the objects is hit, 
    // display "NAME" and "TYPE" parameters as blank
    if (pos >= label.Objects()) {      
      name = "";
      type = "";
      match = true;
      pos = 0;  // Prepare to start over again
    }
    // When we find a blob, fetch its name and type to stick in the parameters
    else if (label.Object(pos).Type() == "Object" && 
             label.Object(pos).HasKeyword("Name") &&
             label.Object(pos).HasKeyword("StartByte") && 
             label.Object(pos).HasKeyword("Bytes")) {
      name = label.Object(pos)["Name"][0].c_str();
      type = label.Object(pos).Name();
      match = true;
      pos++;
    }
    // Nothing's been found yet, keep looking for blobs
    else {      
      pos++;
      cnt++;
    }
  }

  ui.Clear("NAME");
  ui.PutString("NAME",name);
  ui.Clear("TYPE");
  ui.PutString("TYPE", type);
}
