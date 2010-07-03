#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

// Globals and prototypes
bool propagate;
void divfilter (Buffer &in, Buffer &out, QuickFilter &filter);

// The divfilter main routine
void IsisMain() {
  ProcessByQuickFilter p;

  // Open the input cube
  p.SetInputCube("FROM");

  // Setup the output cube
  p.SetOutputCube("TO");

  //Set up boxcar variables (minimum)
  UserInterface &ui = Application::GetUserInterface();
  int lines = ui.GetInteger("LINES");
  int samples = ui.GetInteger("SAMPLES");
  int minimum;
  if (ui.GetString("MINOPT") == "PERCENTAGE") {
    int size = lines * samples;
    double perc = ui.GetInteger("MINIMUM") / 100;
    minimum = (int) (size * perc);
  }
  else {
    minimum = ui.GetInteger("MINIMUM");
  }

  // Find out how to handle special pixels
  propagate = ui.GetBoolean ("PROPAGATE");
  
  // Process each line
  p.StartProcess(divfilter);  // Line processing function
  p.EndProcess();           // Cleanup
}

// Line processing routine
void divfilter (Buffer &in, Buffer &out, QuickFilter &filter) {
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
      if (filter.Average (i) == NULL8) {
        out[i] = NULL8;
      }
      else {
        out[i] = in[i] / filter.Average(i);  // will be NULL if uncomputable or count invalid
      }
    }       
  }
}
