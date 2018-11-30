#include "Isis.h"

#include <complex>

#include <QFile>

#include "FourierTransform.h"
#include "ProcessByTile.h"
#include "Statistics.h"
#include "AlphaCube.h"

using namespace std;
using namespace Isis;

void FFT1(vector<Buffer *> &in, vector<Buffer *> &out);
void FFT2(vector<Buffer *> &in, vector<Buffer *> &out);
void getMinMax(Buffer &in);

FourierTransform fft;
QString tmpMagFileName = "Temporary_IFFT_Magnitude.cub";
QString tmpPhaseFileName = "Temporary_IFFT_Phase.cub";
double HPixel = 0.0, LPixel = 0.0, NPixel = 0.0;

Statistics stats;

void IsisMain() {
  // We will be processing by sample first
  ProcessByTile sProc;

  // Setup the input and output cubes
  Cube *icube = sProc.SetInputCube("FROM");
  int numSamples = fft.NextPowerOfTwo(icube->sampleCount());
  int numLines = fft.NextPowerOfTwo(icube->lineCount());
  int numBands = icube->bandCount();

  sProc.SetTileSize(1, numLines);

  // create an AlphaCube containing the resizing information
  // which will be used during the inverse
  AlphaCube aCube(icube->sampleCount(), icube->lineCount(),
                  icube->sampleCount(), icube->lineCount());

  UserInterface &ui = Application::GetUserInterface();

  QString replacement = ui.GetString("REPLACEMENT");
  if(replacement == "ZEROES") {
    HPixel = 0.0;
    LPixel = 0.0;
    NPixel = 0.0;
  }
  else if(replacement == "MINMAX") {
    sProc.Progress()->SetText("Getting Statistics");
    sProc.ProcessCubeInPlace(getMinMax, false);
    LPixel = stats.Minimum();
    HPixel = stats.Maximum();
    NPixel = 0.0;
  }
  sProc.Progress()->SetText("First pass");

  // The output cube with no attributes and real pixel type
  Isis::CubeAttributeOutput cao;
  cao.setPixelType(Isis::Real);

  sProc.SetOutputCube(tmpMagFileName, cao, numSamples, numLines, numBands);
  sProc.SetOutputCube(tmpPhaseFileName, cao, numSamples, numLines, numBands);

  // Start the sample processing
  sProc.ProcessCubes(&FFT1);
  sProc.Finalize();

  // Then process by line
  ProcessByTile lProc;
  lProc.SetTileSize(numSamples, 1);

  lProc.Progress()->SetText("Second pass");

  // Setup the input and output cubes
  Isis::CubeAttributeInput cai;

  lProc.SetInputCube(tmpMagFileName, cai);
  lProc.SetInputCube(tmpPhaseFileName, cai);

  Cube *ocube = lProc.SetOutputCube("MAGNITUDE");
  lProc.SetOutputCube("PHASE");

  //Start the line proccessing
  lProc.ProcessCubes(&FFT2);

  // Add or update the AlphaCube group
  aCube.UpdateGroup(*ocube);

  // Stop the process and remove the temporary files
  lProc.Finalize();

  QFile::remove(tmpMagFileName);
  QFile::remove(tmpPhaseFileName);
}

// Processing routine for the fft with one input cube
void FFT1(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &image = *in[0];

  int n = image.size();
  std::vector< std::complex<double> > input(n);

  // copy the input data into a complex vector
  for(int i = 0; i < n; i++) {
    if(IsSpecial(image[i])) {
      if(IsHrsPixel(image[i]) || IsHisPixel(image[i])) input[i] = HPixel;
      else if(IsLrsPixel(image[i]) || IsLisPixel(image[i])) input[i] = LPixel;
      else input[i] = NPixel;
    }
    else input[i] = std::complex<double>(image[i]);
  }

  // perform the fourier transform
  std::vector< std::complex<double> > output = fft.Transform(input);
  n = output.size();

  Buffer &realCube = *out[0];
  Buffer &imagCube = *out[1];

  // copy the data into the two output cubes so that it is centered at the origin
  for(int i = 0; i < n / 2; i++) {
    realCube[i] = real(output[n/2+i]);
    imagCube[i] = imag(output[n/2+i]);

    realCube[i+n/2] = real(output[i]);
    imagCube[i+n/2] = imag(output[i]);
  }
}

// Processing routine for the fft with two input cubes
void FFT2(vector<Buffer *> &in, vector<Buffer *> &out) {
  // Set the input cubes
  Buffer &inReal = *in[0];
  Buffer &inImag = *in[1];

  // copy the input buffer into a complex vector
  int n = inReal.size();
  std::vector< std::complex<double> > input(n);

  for(int i = 0; i < n; i++) {
    input[i] = std::complex<double>(inReal[i], inImag[i]);
  }

  // perform the fourier transform
  std::vector< std::complex<double> > output = fft.Transform(input);
  n = output.size();

  Buffer &magCube = *out[0];
  Buffer &phaseCube = *out[1];

  // copy the data into the two output cubes so that it is centered at the origin
  for(int i = 0; i < n / 2; i++) {
    magCube[i] = abs(output[n/2+i]);
    phaseCube[i] = arg(output[n/2+i]);

    magCube[i+n/2] = abs(output[i]);
    phaseCube[i+n/2] = arg(output[i]);
  }
}

void getMinMax(Buffer &in) {
  stats.AddData(in.DoubleBuffer(), in.size());
}
