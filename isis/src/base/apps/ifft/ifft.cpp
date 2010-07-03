#include "Isis.h"
#include <complex>
#include "FourierTransform.h"
#include "ProcessByTile.h"
#include "AlphaCube.h"

using namespace std; 
using namespace Isis;

void IFFT1 (vector<Buffer *> &in, vector<Buffer *> &out);
void IFFT2 (vector<Buffer *> &in, vector<Buffer *> &out);

FourierTransform fft;
string tmpMagFilename = "Temporary_IFFT_Magnitude.cub";
string tmpPhaseFilename = "Temporary_IFFT_Phase.cub";

void IsisMain() {
  // We will be processing by line first
  ProcessByTile lProc;
  lProc.Progress()->SetText("First pass");

  // Get the original cubes dimensions
  UserInterface &ui = Application::GetUserInterface();
  Pvl lab;
  lab.Read(ui.GetFilename("MAGNITUDE"));
  AlphaCube acube(lab);
  int initSamples = acube.BetaSamples();
  int initLines = acube.BetaLines();

  // Setup the input and output cubes
  Cube *magCube = lProc.SetInputCube("MAGNITUDE");
  Cube *phaseCube = lProc.SetInputCube("PHASE");

  int numSamples = magCube->Samples();
  int numLines = magCube->Lines();
  int numBands = magCube->Bands();

  // error checking for valid input cubes
  // i.e. the dimensions of the magnitude and phase cubes
  // are the same and are powers of two
  if (!fft.IsPowerOfTwo(numSamples) || !fft.IsPowerOfTwo(numLines) 
      || magCube->Samples()!=phaseCube->Samples() || magCube->Lines()!=phaseCube->Lines()) {
    cerr << "Invalid Cubes: the dimensions of both cubes must be equal powers of 2." << endl;
    return;
  }

  lProc.SetTileSize(numSamples, 1);

  Isis::CubeAttributeOutput cao;

  lProc.SetOutputCube (tmpMagFilename, cao, numSamples, numLines, numBands);
  lProc.SetOutputCube (tmpPhaseFilename, cao, numSamples, numLines, numBands);

  // Start the line processing
  lProc.StartProcess(IFFT2);
  lProc.EndProcess();

  // Then process by sample
  ProcessByTile sProc;
  sProc.Progress()->SetText("Second pass");
  sProc.SetTileSize(1, numLines);

  // Setup the input and output cubes
  Isis::CubeAttributeInput cai;

  sProc.SetInputCube(tmpMagFilename, cai);
  sProc.SetInputCube(tmpPhaseFilename, cai);

  // the final output cube is cropped back to the original size
  sProc.SetOutputCube("TO", initSamples, initLines, numBands);

  //Start the sample proccessing
  sProc.StartProcess(IFFT1);
  sProc.EndProcess();

  remove(tmpMagFilename.c_str());
  remove(tmpPhaseFilename.c_str());
}

// Processing routine for the inverse fft
void IFFT1 (vector<Buffer *> &in, vector<Buffer *> &out)
{
  Buffer &inReal = *in[0];
  Buffer &inImag = *in[1];

  int n = inReal.size();
  vector< complex<double> > input(n);

  // copy and rearrange the data to fit the algorithm
  // the image is centered at zero, the array begins at zero
  for (int i=0; i<n/2; i++) {
    input[i] = complex<double>(inReal[i+n/2], inImag[i+n/2]);
    input[i+n/2] = complex<double>(inReal[i], inImag[i]);
  }

  // compute the inverse fft
  vector< complex<double> > output = fft.Inverse(input);

  Buffer &image = *out[0];
  // and copy the result to the output cube
  for ( int i=0; i<n; i++) {
    image[i]=real(output[i]);
  }
}

// Processing routine for the inverse fft with two output cubes
void IFFT2 (vector<Buffer *> &in, vector<Buffer *> &out)
{
  Buffer &mag = *in[0];
  Buffer &phase = *in[1];

  int n = mag.size();
  vector< complex<double> > input(n);

  // copy and rearrange the data to fit the algorithm
  // the image is centered at zero, the array begins at zero
  for (int i=0; i<n/2; i++) {
    input[i] = complex<double>(polar(mag[i+n/2], phase[i+n/2]));
    input[i+n/2] = complex<double>(polar(mag[i], phase[i]));
  }

  // compute the inverse fft
  vector< complex<double> > output = fft.Inverse(input);

  Buffer &realCube = *out[0];
  Buffer &imagCube = *out[1];
  // and copy the result to the output cubes
  for ( int i=0; i<n; i++) {
    realCube[i] = real(output[i]);
    imagCube[i] = imag(output[i]);
  }
}
