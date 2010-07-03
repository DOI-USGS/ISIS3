#include "Isis.h"
#include "ProcessByBrick.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void BandTrim (Buffer &in, Buffer &out);

void IsisMain()
{
  ProcessByBrick p;
  Cube *icube = p.SetInputCube("FROM");
  p.SetBrickSize(1, 1, icube->Bands());
  p.SetOutputCube ("TO");
  p.StartProcess(BandTrim);
  p.EndProcess();
}

// Trim spectral pixels if anyone of them is null
void BandTrim (Buffer &in, Buffer &out){
  // Copy input to output and check to see if we should null
  bool nullPixels = false;
  for (int i=0; i<in.size(); i++) {
    out[i] = in[i];
    if (in[i] == Isis::Null) nullPixels = true;
  } 

  // Null all pixels in the spectra if necessary
  if (nullPixels) { 
    for (int i=0; i<in.size(); i++) {
      out[i] = Isis::Null;
    } 
  }
}
