#include <cmath>
#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

void poly (Buffer &in, Buffer &out);

double coefficients[8];
double add;
int order;

void IsisMain() {
  // We will be processing by line
   ProcessByLine p;

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");

  //  Get user parameters
  UserInterface &ui = Application::GetUserInterface(); 
  coefficients[0] = ui.GetDouble ("MULT1");
  coefficients[1] = ui.GetDouble ("MULT2");
  coefficients[2] = ui.GetDouble ("MULT3");
  coefficients[3] = ui.GetDouble ("MULT4");
  coefficients[4] = ui.GetDouble ("MULT5");
  coefficients[5] = ui.GetDouble ("MULT6");
  coefficients[6] = ui.GetDouble ("MULT7");
  coefficients[7] = ui.GetDouble ("MULT8");
  add = ui.GetDouble ("ADD");

  // Determine the order
  for (order=7; order>=0; order--) {
    if (coefficients[order] != 0.0) break;
  }
  order += 1;

  // Start the processing
  p.StartProcess(poly);
  p.EndProcess();
}

// Line processing routine
void poly (Buffer &in, Buffer &out) { 
  // Loop for each pixel in the line. 
  for (int i = 0; i < in.size(); i++) {
    if (IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = add;
      for (int j = 1; j <= order; j++) {
        out[i] += pow(in[i], j) * coefficients[j-1]; 
      }
    }
  }
                
}
