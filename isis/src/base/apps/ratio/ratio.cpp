#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void ratio (vector<Buffer *> &in,
            vector<Buffer *> &out);

void IsisMain() {
  ProcessByLine p;
  p.SetInputCube("NUMERATOR");
  p.SetInputCube("DENOMINATOR");
  p.SetOutputCube ("TO");
  p.StartProcess(ratio);
  p.EndProcess();
}

// Line processing routine
void ratio (vector<Buffer *> &in,
            vector<Buffer *> &out) {
  Buffer &num = *in[0];
  Buffer &den = *in[1];
  Buffer &rat = *out[0];

  // Loop for each pixel in the line. Check
  // for special pixels and if any are found the
  // output will be set to NULL.
  for (int i=0; i<num.size(); i++) {
    if (IsSpecial(num[i])) {
      rat[i] = NULL8;
    }
    else if (IsSpecial(den[i])) {
      rat[i] = NULL8;
    }
    else if (den[i] == 0.0) {
      rat[i] = NULL8;     
    }
    else {
      rat[i] = num[i] / den[i];
    }
  }
}
