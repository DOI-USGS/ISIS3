#include "Isis.h"

#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void circle(Buffer &in, Buffer &out);
double cline, csamp, radius; // Global variables

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output cubes
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  // Compute the defaults for user parameters
  cline = icube->getLineCount() / 2;
  csamp = icube->getSampleCount() / 2;
  radius = (cline < csamp) ? cline : csamp;

  // Override the defaults if the user entered a value
  UserInterface &ui = Application::GetUserInterface();

  bool defByRadius = (ui.GetString("DEFINITION") == "CENTERRAD");
  bool defByEdge = !defByRadius;

  //If user enters center coordinates and radius
  if (defByRadius) {
    if ( ui.WasEntered("LINE") ) cline = ui.GetDouble("LINE");
    if ( ui.WasEntered("SAMPLE") ) csamp = ui.GetDouble("SAMPLE");
    if ( ui.WasEntered("RADIUS") ) radius = ui.GetDouble("RADIUS");
  }

  //if user enters 3 points on the edge of the circle
  if (defByEdge) {
    double y1 = ui.GetDouble("LINE1");
    double y2 = ui.GetDouble("LINE2");
    double y3 = ui.GetDouble("LINE3");
    double x1 = ui.GetDouble("SAMP1");
    double x2 = ui.GetDouble("SAMP2");
    double x3 = ui.GetDouble("SAMP3");

    // Compute the center line/samp and radius of the circle
    double x21 = x2 - x1;
    double y21 = y2 - y1;
    double x31 = x3 - x1;
    double y31 = y3 - y1;
    double den = 2.0 * (x21 * y31 - x31 * y21);
    if(den == 0.0) {
      string message = "The three points lie on a line so a circle can not be computed";
      throw IException(IException::User, message, _FILEINFO_);
    }

    double sq2 = x21 * x21 + y21 * y21;
    double sq3 = x31 * x31 + y31 * y31;
    csamp = (sq2 * y31 - sq3 * y21) / den;
    cline = (sq3 * x21 - sq2 * x31) / den;

    radius = sqrt(csamp * csamp + cline * cline);
    csamp += x1;
    cline += y1;
  }

  // Start the processing
  p.StartProcess(circle);
  p.EndProcess();
}

// Line processing routine
void circle(Buffer &in, Buffer &out) {
  // Compute part of the distance (doesn't vary since the line is constant)
  double dist, partA, partB;
  partA = cline - (double) in.Line();
  partA *= partA;

  // Loop for each pixel in the line.
  for(int i = 0; i < in.size(); i++) {
    // Compute the rest of the distance
    partB = csamp - (double) in.Sample(i);
    partB *= partB;
    dist = partA + partB;
    if(dist < 0.0) dist = 0.0;  // Shouldn't happen
    dist = sqrt(dist);

    // Mask everything outside the radius and keep what is inside
    if(dist <= radius) {
      out[i] = in[i];
    }
    else {
      out[i] = NULL8;
    }
  }
}
