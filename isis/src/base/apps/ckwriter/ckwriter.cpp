#include <cmath>

#include "Isis.h"
#include "FileList.h"
#include "Cube.h"
#include "Process.h"
#include "Pvl.h"
#include "SpiceKernel.h"
#include "SpiceSegment.h"

using namespace std; 
using namespace Isis;

void IsisMain() {
  Process p;

  // Get the list of names of input CCD cubes to stitch together
  FileList flist;
  UserInterface &ui = Application::GetUserInterface();
  if (ui.WasEntered("FROM")) flist.push_back(ui.GetFilename("FROM")); 
  if (ui.WasEntered("FROMLIST")) flist.Read(ui.GetFilename("FROMLIST"));
  if (flist.size() < 1) {
    string msg = "Files must be specified in FROM and/or FROMLIST - none found!";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  SpiceKernel kernel;
  Progress prog;
  prog.SetMaximumSteps(flist.size());
  prog.CheckStatus();

  for (unsigned int i = 0 ; i < flist.size() ; i++) {
    // Set the input image, get the camera model, and a basic mapping
    // group
    kernel.add(flist[i]);

  //  p.ClearInputCubes();
    prog.CheckStatus();
  }

  // Write the output file if requested
  if (ui.WasEntered("TO")) {
    string comfile("");
    if (ui.WasEntered("COMFILE")) comfile = ui.GetFilename("COMFILE");
    int cktype = ui.GetInteger("CKTYPE");

    kernel.write(ui.GetFilename("TO"), comfile, cktype);
  }

  p.EndProcess();
}
