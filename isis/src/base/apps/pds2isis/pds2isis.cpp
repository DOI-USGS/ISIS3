#include "Isis.h"
#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Filename.h"

using namespace std; 
using namespace Isis;

void IsisMain ()
{
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  string labelFile = ui.GetFilename("FROM");
  string imageFile("");
  if (ui.WasEntered("IMAGE")) {
    imageFile = ui.GetFilename("IMAGE");
  }

  p.SetPdsFile (labelFile, imageFile, label);
  Cube *ocube = p.SetOutputCube ("TO");

  // Get user entered special pixel ranges
  if (ui.GetBoolean("SETNULLRANGE")) {
    p.SetNull(ui.GetDouble("NULLMIN"),ui.GetDouble("NULLMAX"));
  }
  if (ui.GetBoolean("SETHRSRANGE")) {
    p.SetHRS(ui.GetDouble("HRSMIN"),ui.GetDouble("HRSMAX"));
  }
  if (ui.GetBoolean("SETHISRANGE")) {
    p.SetHIS(ui.GetDouble("HISMIN"),ui.GetDouble("HISMAX"));
  }
  if (ui.GetBoolean("SETLRSRANGE")) {
    p.SetLRS(ui.GetDouble("LRSMIN"),ui.GetDouble("LRSMAX"));
  }
  if (ui.GetBoolean("SETLISRANGE")) {
    p.SetLIS(ui.GetDouble("LISMIN"),ui.GetDouble("LISMAX"));
  }

  // Export the cube
  p.StartProcess ();

  // Get as many of the other labels as we can
  Pvl otherLabels;
  p.TranslatePdsProjection(otherLabels);
  if (p.IsIsis2()) {
    p.TranslateIsis2Labels (otherLabels);
  }
  else {
    p.TranslatePdsLabels (otherLabels);
  }

  if (otherLabels.HasGroup("Mapping") &&
      (otherLabels.FindGroup("Mapping").Keywords() > 0)) {
    ocube->PutGroup(otherLabels.FindGroup("Mapping"));
  }
  if (otherLabels.HasGroup("Instrument") &&
      (otherLabels.FindGroup("Instrument").Keywords() > 0)) {
    ocube->PutGroup(otherLabels.FindGroup("Instrument"));
  }
  if (otherLabels.HasGroup("BandBin") &&
      (otherLabels.FindGroup("BandBin").Keywords() > 0)) {
    ocube->PutGroup(otherLabels.FindGroup("BandBin"));
  }
  if (otherLabels.HasGroup("Archive") &&
      (otherLabels.FindGroup("Archive").Keywords() > 0)) {
    ocube->PutGroup(otherLabels.FindGroup("Archive"));
  }

  p.EndProcess ();

  return;
}

