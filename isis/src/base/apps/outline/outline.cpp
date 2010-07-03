#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

// Line processing routine
void outline (Buffer &in, Buffer &out);

double *lastLine;
double bdn;
bool bimage;
bool bclear;
int nl;
 
void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output cubes
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube ("TO");

  // Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  bdn = ui.GetDouble ("BOUNDARY");
  bimage = ui.GetBoolean("EDGES");
  bclear = ui.GetBoolean("CLEAR");

  // Allocate space for a work buffer
  lastLine = new double[icube->Samples()];
  nl = icube->Lines();

  // Start the processing
  p.StartProcess(outline);
  p.EndProcess();

  // Cleanup
  delete [] lastLine;
}


//  Line processing routine
void outline (Buffer &in,Buffer &out) 
{
  // Handle things differently for the 1st line
  if (in.Line() == 1) {
    for (int i=1; i<in.size(); i++) {
      if (in[i] != in[i-1]) {
        out[i] = bdn;
      }
      else if (bclear) {
        out[i] = NULL8;
      }
      else {
        out[i] = in[i];
      }
    }

    // handle first sample
    if (bclear) {
      out[0] = NULL8;
    }
    else {
      out[0] = in[0];
    }
  }

  // Handle lines 2 - NL
  else {
    for (int i=1; i<in.size(); i++) {
      if ((in[i] != in[i-1]) || (in[i] != lastLine[i])) {
        out[i] = bdn;
      }
      else if (bclear) {
        out[i] = NULL8;
      }
      else {
        out[i] = in[i];
      }
    }
    
    // handle first sample
    if (in[0] != lastLine[0]) {
      out[0] = bdn;      
    }
    else if (bclear) {
      out[0] = NULL8;
    }
    else {
      out[0] = in[0];
    }
  }

  // If the user wants the edge of the image bounded
  // then do it

  if (bimage) {
    out[0] = bdn;
    out[in.size()-1] = bdn;

    if ((in.Line() == 1) || (in.Line() == nl)) {
      for (int i=0; i<in.size(); i++) {
        out[i] = bdn;
      }
    }
  }

  // Save the input line
  for (int i=0; i<in.size(); i++) {
    lastLine[i] = in[i];
  }
}


