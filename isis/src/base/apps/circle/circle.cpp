#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void circle (Buffer &in, Buffer &out);
double cline,csamp,radius;  // Global variables

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output cubes
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube ("TO");
  
  // Compute the defaults for user parameters
  cline = icube->Lines() / 2;
  csamp = icube->Samples() / 2;
  radius = (cline < csamp) ? cline : csamp;

  // Override the defaults if the user entered a value
  UserInterface &ui = Application::GetUserInterface();
  if (ui.WasEntered ("LINE")) cline = ui.GetDouble ("LINE");
  if (ui.WasEntered ("SAMPLE")) csamp = ui.GetDouble ("SAMPLE");
  if (ui.WasEntered ("RADIUS")) radius = ui.GetDouble ("RADIUS");

  // Start the processing
  p.StartProcess(circle);
  p.EndProcess();
}

// Line processing routine
void circle (Buffer &in, Buffer &out) {
  // Compute part of the distance (doesn't vary since the line is constant)
  double dist, partA, partB;
  partA = cline - (double) in.Line();
  partA *= partA; 

  // Loop for each pixel in the line. 
  for (int i=0; i<in.size(); i++) {
    // Compute the rest of the distance
    partB = csamp - (double) in.Sample(i);
    partB *= partB;
    dist = partA + partB;
    if (dist < 0.0) dist = 0.0; // Shouldn't happen
    dist = sqrt(dist);

    // Mask everything outside the radius and keep what is inside
    if (dist <= radius) {
      out[i] = in[i];
    }
    else {
      out[i] = NULL8;
    }
  }
}
