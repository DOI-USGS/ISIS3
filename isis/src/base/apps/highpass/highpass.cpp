#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "UserInterface.h"

using namespace std; 
using namespace Isis;

// Globals and prototypes
bool propagate;
double addback;
void highpass (Buffer &in, Buffer &out, QuickFilter &filter);

// The highpass main routine
void IsisMain() {
  ProcessByQuickFilter p;

  // Open the input cube
  p.SetInputCube("FROM");

  // Setup the output cube
  p.SetOutputCube("TO");

  // Find out how to handle special pixels
  UserInterface &ui = Application::GetUserInterface();
  propagate = ui.GetBoolean ("PROPAGATE");

  // Get the addback
  addback = ui.GetDouble ("ADDBACK") / 100.0;

  //Set the boxcar parameters
  int lines = ui.GetInteger("LINES");
  int samples = ui.GetInteger("SAMPLES");
  double low = -DBL_MAX;
  double high = DBL_MAX;
  int minimum;
  if (ui.WasEntered("LOW")) {
    low = ui.GetDouble("LOW");
  }
  if (ui.WasEntered("HIGH")) {
    high = ui.GetDouble("HIGH");
  }
  if (ui.GetString("MINOPT") == "PERCENTAGE") {
    int size = lines * samples;
    double perc = ui.GetDouble("MINIMUM") / 100;
    minimum = (int) (size * perc);
  }
  else {
    minimum = (int)ui.GetDouble("MINIMUM");
  }
  p.SetFilterParameters(samples, lines, low, high, minimum);
  
  // Process each line
  p.StartProcess(highpass);  // Line processing function
  p.EndProcess();           // Cleanup
}

// Line processing routine
void highpass (Buffer &in, Buffer &out, QuickFilter &filter) {
  for (int i=0; i<filter.Samples(); i++) {
    // We have a special pixel
    if (IsSpecial(in[i])) {
      if (propagate) {
        out[i] = in[i];
      }
      else {
        out[i] = NULL8;
      }
    }
    // We have a normal pixel
    else {
      out[i] = filter.Average(i);  // will be NULL if uncomputable or count invalid
      // If the average could be computed then subtract it from the input
      if (!IsSpecial(out[i])) out[i] = in[i] - out[i] + addback * in[i];
    }
  }
}
