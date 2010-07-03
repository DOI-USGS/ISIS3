#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void specadd (vector<Buffer *> &in, vector<Buffer *> &out);

void IsisMain() {
  ProcessByLine p;

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetInputCube("MATCH");
  p.SetOutputCube ("TO" );

  p.StartProcess(specadd);
  p.EndProcess();
}

// Line processing routine
void specadd (vector<Buffer *> &in, vector<Buffer *> &out)
{
  Buffer &from = *in[0];
  Buffer &match = *in[1];
  Buffer &to = *out[0];

  // copy the original image unless it's a special pixel
  // in which case the information is copied from the match cube
  for (int i=0; i<from.size(); i++) {
    if (IsSpecial(match[i])) to[i] = match[i];
    else to[i] = from[i];
  }
}
