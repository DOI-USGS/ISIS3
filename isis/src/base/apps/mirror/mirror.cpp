#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void mirror (Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");
  
  // Start the processing
  p.StartProcess(mirror);
  p.EndProcess();
}

// Line processing routine
void mirror (Buffer &in, Buffer &out) {
  // Loop and flip pixels in the line.
  int index = in.size() - 1;
  for (int i=0; i<in.size(); i++) {
    out[i] = in[index - i];
  }


}
