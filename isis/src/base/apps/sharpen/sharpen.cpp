#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "UserInterface.h"

using namespace std; 
using namespace Isis;

// Globals and prototypes
bool propagate;
void sharpen (Buffer &in, Buffer &out, QuickFilter &filter);

// The sharpen main routine
void IsisMain() {
  ProcessByQuickFilter p;

  // Open the input cube
  p.SetInputCube("FROM");

  // Setup the output cube
  p.SetOutputCube("TO");

  // Find out how to handle special pixels
  UserInterface &ui = Application::GetUserInterface();
  propagate = ui.GetBoolean ("PROPAGATE");

  // Process each line
  p.StartProcess(sharpen);  // Line processing function
  p.EndProcess();           // Cleanup
}

// Line processing routine
void sharpen (Buffer &in, Buffer &out, QuickFilter &filter) {
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
      out[i] = filter.Average(i);  
      // out[i] will be NULL if uncomputable or count invalid
      // If the average could be computed then sharpen by adding the
      // original + hpf
      if (!IsSpecial(out[i])) out[i] = in[i] + (in[i] - out[i]);
    }
  }
}
