#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

// Glocal declarations
void makecube (Buffer &out);
double value;

void IsisMain() {
  // Create a process by line object
  ProcessByLine p;

  // Get the value to put in the cube
  UserInterface &ui = Application::GetUserInterface();
  string pixels = ui.GetString("PIXELS");
  if (pixels == "NULL") {
    value = NULL8;
  }
  else if (pixels == "LIS") {
    value = LOW_INSTR_SAT8;
  }
  else if (pixels == "LRS") {
    value = LOW_REPR_SAT8;
  }
  else if (pixels == "HIS") {
    value = HIGH_INSTR_SAT8;
  }
  else if (pixels == "HRS") {
    value = HIGH_REPR_SAT8;
  }
  else {
    value = ui.GetDouble("VALUE");
  }

  // Need to pick good min/maxs to ensure the user's value
  // doesn't get saturated
  CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
  if (IsValidPixel(value)) {
    if (value == 0.0) {
      att.Minimum(value);
      att.Maximum(value+1.0);
    }  
    if (value < 0.0) {
      att.Minimum(value);
      att.Maximum(-value);
    }
    else {
      att.Minimum(-value);
      att.Maximum(value);
    }
  }
  else {
    att.Minimum(0.0);
    att.Maximum(1.0);
  }

  // Get the size of the cube and create the cube
  int samps = ui.GetInteger("SAMPLES");
  int lines = ui.GetInteger("LINES");
  int bands = ui.GetInteger("BANDS");
  p.SetOutputCube (ui.GetFilename("TO"),att,samps,lines,bands);

  // Make the cube
  p.StartProcess(makecube);
  p.EndProcess();
}

void makecube (Buffer &out) {
  for (int i=0; i<out.size(); i++) {
    out[i] = (double) value;
  }
}
