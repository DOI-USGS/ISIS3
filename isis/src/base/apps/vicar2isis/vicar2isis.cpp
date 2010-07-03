#include "Isis.h"
#include "ProcessImportVicar.h"
#include "UserInterface.h"
#include "Pvl.h"

using namespace std; 
using namespace Isis;

void IsisMain ()
{
  UserInterface &ui = Application::GetUserInterface ();
  ProcessImportVicar p;

  // Set Special Pixel ranges
  if (ui.GetBoolean("SETNULLRANGE")) {
    p.SetNull(ui.GetDouble("NULLMIN"),ui.GetDouble("NULLMAX"));
  }
  if (ui.GetBoolean("SETHRSRANGE")) {
    p.SetHRS(ui.GetDouble("HRSMIN"),ui.GetDouble("HRSMAX"));
  }
  if (ui.GetBoolean("SETLRSRANGE")) {
    p.SetLRS(ui.GetDouble("LRSMIN"),ui.GetDouble("LRSMAX"));
  }

  Pvl vicLab;
  p.SetVicarFile(ui.GetFilename ("FROM"),vicLab);
  p.SetOutputCube("TO");

  p.StartProcess ();
  p.EndProcess ();

  return;
}

