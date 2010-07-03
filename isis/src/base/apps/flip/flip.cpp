#include "Isis.h"
#include "ProcessBySample.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void flip (Buffer &in, Buffer &out);

void IsisMain() {
  // We will be processing by line
  ProcessBySample p;

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");
  
  // Start the processing
  p.StartProcess(flip);
  p.EndProcess();
}

// Line processing routine
void flip (Buffer &in, Buffer &out) {
  // Loop and flip pixels in the line.
  int index = in.size() - 1;
  for (int i=0; i<in.size(); i++) {
    out[i] = in[index - i];
  }


}
