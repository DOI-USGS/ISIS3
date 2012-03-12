#include <cmath>

#include "Isis.h"
#include "FileList.h"
#include "Cube.h"
#include "Process.h"
#include "Pvl.h"
#include "SpiceKernel.h"
#include "SpkSegment.h"
#include "Commentor.h"
#include "SpkKernelWriter.h"

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
    throw IException(IException::User,msg,_FILEINFO_);
  }

  SpiceKernel<SpkSegment> kernel;
  Progress prog;
  prog.SetMaximumSteps(flist.size());
  prog.CheckStatus();

  for (unsigned int i = 0 ; i < flist.size() ; i++) {
    // Add and process each image
    kernel.add(SpkSegment(flist[i]));
    prog.CheckStatus();
  }

  //  Process CK kernel requests
  string comfile("");
  if (ui.WasEntered("COMFILE")) comfile = ui.GetFilename("COMFILE");

  SpkKernelWriter kwriter(ui.GetInteger("TYPE"));

  // Write the output file if requested
  if (ui.WasEntered("TO")) {
    kwriter.write(kernel, ui.GetFilename("TO"), comfile);
  }

  // Write a summary of the documentation
  if (ui.WasEntered("SUMMARY")) {
    string fFile = Filename(ui.GetFilename("SUMMARY")).Expanded();
    ofstream os;
    os.open(fFile.c_str(),ios::out);
    if (!os) {
      string mess = "Cannot create SPK SUMMARY output file " + fFile;
      throw IException(IException::User, mess, _FILEINFO_);
    }
    os << kwriter.getComment(kernel, comfile) << endl;
    os.close();
  }
  p.EndProcess();
}
