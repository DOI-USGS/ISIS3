#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "UserInterface.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

// prototypes and globals
bool trimmed;
void trimfilter (Buffer &in, Buffer &out, QuickFilter &filter);

// The trimfilter main routine
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
  double low = -DBL_MAX;
  double high = DBL_MAX;
  int minimum;
  if (ui.GetString("MINOPT") == "PERCENTAGE") {
    int size = lines * samples;
    double perc = ui.GetDouble("MINIMUM") / 100;
    minimum = (int) (size * perc);
  }
  else {
    minimum = (int) ui.GetDouble("MINIMUM");
  }
  p.SetFilterParameters(samples, lines, low, high, minimum);

  // Process each line
  trimmed = false;
  p.StartProcess(trimfilter);  // Line processing function
  p.EndProcess();              // Cleanup

  // If trimming did not occur tell the user
  if (!trimmed) {
    string msg = "Your selected parameters did not trim any data from the cube";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
}

// Line processing routine
void trimfilter (Buffer &in, Buffer &out, QuickFilter &filter) {
  for (int i=0; i<filter.Samples(); i++) {
    if (filter.Count(i) >= filter.MinimumPixels()) {
      out[i] = in[i];
    }
    else {
      trimmed = true;
      out[i] = NULL8;
    }
  }
}
