#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "UserInterface.h"

using namespace std; 
using namespace Isis;

// Globals and prototypes
bool propagate;
bool stdDev;
void svfilter (Buffer &in, Buffer &out, QuickFilter &filter);

// The svfilter main routine
void IsisMain() {
  ProcessByQuickFilter p;

  // Open the input cube
  p.SetInputCube("FROM");

  // Setup the output cube
  p.SetOutputCube("TO");

  // Find out how to handle special pixels
  UserInterface &ui = Application::GetUserInterface();
  propagate = ui.GetBoolean ("PROPAGATE");
  string filt = ui.GetString ("FILTER"); 
  stdDev = false;
  if (filt == "STDDEV") stdDev = true;
  
  // Process each line
  p.StartProcess(svfilter);  // Line processing function
  p.EndProcess();           // Cleanup
}

// Line processing routine
void svfilter (Buffer &in, Buffer &out, QuickFilter &filter) {
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
      out[i] = filter.Variance(i);  // will be NULL if uncomputable or count invalid
      // If the standard deviation flag is set, return standard deviation instead of
      // variance.
      if (stdDev && IsValidPixel(out[i])) out[i] = sqrt (out[i]);
    }
  }
}
