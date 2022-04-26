#include "Isis.h"
#include "ProcessByBrick.h"
#include "ProcessByLine.h"
#include "ProcessBySample.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

// Glocal declarations
void GreyScale(Buffer &out);
double dn1;
double dn2;

void IsisMain() {
  // Create a process by line and ui objects
  UserInterface &ui = Application::GetUserInterface();

  // Get the values for the output cube
  dn1 = ui.GetDouble("BEGINDN");
  dn2 = ui.GetDouble("ENDDN");

  // Need to pick good min/maxs to ensure the user's value
  // doesn't get saturated
  CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
  if(IsValidPixel(dn1) && IsValidPixel(dn2)) {
    if(dn1 <= dn2) {
      att.setMinimum(dn1);
      att.setMaximum(dn2);
    }
    else {
      att.setMinimum(dn2);
      att.setMaximum(dn1);
    }
  }
  else {
    throw IException(IException::User, "Must enter valid pixel DN values.", _FILEINFO_);
  }

  // Get the size of the cube and create the cube
  int samps = ui.GetInteger("SAMPLES");
  int lines = ui.GetInteger("LINES");

  // Create a ProcessByBrick pointer
  ProcessByBrick *p;
  // Determine whether to process by line or by sample
  if(ui.GetString("DIRECTION") == "HORIZONTAL") {
    p = new ProcessByLine;
    // we will only process 1 line at a time
    p->SetBrickSize(samps, 1, 1);
  }
  else {
    p = new ProcessBySample;
    // we will only process 1 sample at a time
    p->SetBrickSize(1, lines, 1);
  }
  //Make the cube
  p->SetOutputCube(ui.GetCubeName("TO"), att, samps, lines);
  p->StartProcess(GreyScale);
  p->EndProcess();

}

void GreyScale(Buffer &out) {
  int size = out.size();
  for(int i = 0; i < size; i++) {
    out[i] = dn1 + (dn2 - dn1) * i / (size - 1);
  }
}
