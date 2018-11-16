#include "Isis.h"

#include <complex>

#include "AlphaCube.h"
#include "FourierTransform.h"
#include "ProcessByTile.h"

using namespace std;
using namespace Isis;

void IFFT1(vector<Buffer *> &in, vector<Buffer *> &out);
void IFFT2(vector<Buffer *> &in, vector<Buffer *> &out);

FourierTransform fft;
QString tmpMagFileName = "Temporary_IFFT_Magnitude.cub";
QString tmpPhaseFileName = "Temporary_IFFT_Phase.cub";

void IsisMain() {
  // We will be processing by line first
  ProcessByTile lProc;
  lProc.Progress()->SetText("First pass");

  // Setup the input and output cubes
  Cube *magCube = lProc.SetInputCube("MAGNITUDE");
  Cube *phaseCube = lProc.SetInputCube("PHASE");

  AlphaCube acube(*magCube);
  int initSamples = acube.BetaSamples();
  int initLines = acube.BetaLines();

  int numSamples = magCube->sampleCount();
  int numLines = magCube->lineCount();
  int numBands = magCube->bandCount();

  // error checking for valid input cubes
  // i.e. the dimensions of the magnitude and phase cubes
  // are the same and are powers of two
  if(!fft.IsPowerOfTwo(numSamples) || !fft.IsPowerOfTwo(numLines)
      || magCube->sampleCount() != phaseCube->sampleCount()
      || magCube->lineCount() != phaseCube->lineCount()) {
    cerr << "Invalid Cubes: the dimensions of both cubes must be equal"
            " powers of 2." << endl;
    return;
  }

  lProc.SetTileSize(numSamples, 1);

  Isis::CubeAttributeOutput cao;

  lProc.SetOutputCube(tmpMagFileName, cao, numSamples, numLines, numBands);
  lProc.SetOutputCube(tmpPhaseFileName, cao, numSamples, numLines, numBands);

  // Start the line processing
  lProc.ProcessCubes(&IFFT2);
  lProc.Finalize();

  // Then process by sample
  ProcessByTile sProc;
  sProc.Progress()->SetText("Second pass");
  sProc.SetTileSize(1, numLines);

  // Setup the input and output cubes
  Isis::CubeAttributeInput cai;

  sProc.SetInputCube(tmpMagFileName, cai);
  sProc.SetInputCube(tmpPhaseFileName, cai);

  // the final output cube is cropped back to the original size
  Cube *outputCube = sProc.SetOutputCube("TO", initSamples, initLines, numBands);

  //Start the sample proccessing
  sProc.ProcessCubes(&IFFT1);

  // Remove the AlphaCube if the alpha and beta dimensions match the output cube dimensions
  // (i.e. remove this group if it didn't exist before running fft).
  int outputSamples = outputCube->sampleCount();
  int outputLines = outputCube->lineCount();
  if (initSamples == outputSamples
      && initLines == outputLines 
      && acube.AlphaSamples() == outputSamples
      && acube.AlphaLines() == outputLines) {
    Pvl *label = outputCube->label();
    PvlObject &isisCube = label->findObject("IsisCube");
    if (isisCube.hasGroup("AlphaCube")) {
      isisCube.deleteGroup("AlphaCube");
    }
  }

  sProc.Finalize();

  remove(tmpMagFileName.toLatin1().data());
  remove(tmpPhaseFileName.toLatin1().data());
}

// Processing routine for the inverse fft
void IFFT1(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inReal = *in[0];
  Buffer &inImag = *in[1];

  int n = inReal.size();
  vector< complex<double> > input(n);

  // copy and rearrange the data to fit the algorithm
  // the image is centered at zero, the array begins at zero
  for(int i = 0; i < n / 2; i++) {
    input[i] = complex<double>(inReal[i+n/2], inImag[i+n/2]);
    input[i+n/2] = complex<double>(inReal[i], inImag[i]);
  }

  // compute the inverse fft
  vector< complex<double> > output = fft.Inverse(input);

  Buffer &image = *out[0];
  // and copy the result to the output cube
  for(int i = 0; i < n; i++) {
    image[i] = real(output[i]);
  }
}

// Processing routine for the inverse fft with two output cubes
void IFFT2(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &mag = *in[0];
  Buffer &phase = *in[1];

  int n = mag.size();
  vector< complex<double> > input(n);

  // copy and rearrange the data to fit the algorithm
  // the image is centered at zero, the array begins at zero
  for(int i = 0; i < n / 2; i++) {
    input[i] = complex<double>(polar(mag[i+n/2], phase[i+n/2]));
    input[i+n/2] = complex<double>(polar(mag[i], phase[i]));
  }

  // compute the inverse fft
  vector< complex<double> > output = fft.Inverse(input);

  Buffer &realCube = *out[0];
  Buffer &imagCube = *out[1];
  // and copy the result to the output cubes
  for(int i = 0; i < n; i++) {
    realCube[i] = real(output[i]);
    imagCube[i] = imag(output[i]);
  }
}
