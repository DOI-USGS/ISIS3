#include "Isis.h"
#include "AlphaCube.h"
#include "iException.h"
#include "iString.h"
#include "LineManager.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "SubArea.h"

#include <cmath>

using namespace std;
using namespace Isis;

void average (Buffer &out);
void nearest (Buffer &out);

Cube * cube;
LineManager *in;
double sscale,lscale;
double vper;
int ins,inl,inb;
int ons,onl;
double line;
int iline;
int sb;
string replaceMode;
std::vector<string> bands;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;
  cube = new Cube;

  // To propogate labels, set input cube,
  // this cube will be cleared after output cube is set.
  p.SetInputCube("FROM");

  // Setup the input and output cubes
  UserInterface &ui = Application::GetUserInterface();
  replaceMode = ui.GetAsString("VPER_REPLACE");
  CubeAttributeInput cai(ui.GetAsString("FROM"));
  bands = cai.Bands();

  string from = ui.GetFilename ("FROM");
  cube->Open(from);

  ins = cube->Samples();
  inl = cube->Lines();
  inb = bands.size();

  if (inb == 0) {
    inb = cube->Bands();
    for (int i = 1; i<=inb; i++) {
      bands.push_back((iString)i);
    }
  }

  string alg = ui.GetString("ALGORITHM");
  vper = ui.GetDouble ("VALIDPER")/100.;

  if (ui.GetString("MODE") == "TOTAL") {
    ons = ui.GetInteger ("ONS");
    onl = ui.GetInteger ("ONL");
    sscale = (double)ins / (double)ons;
    lscale = (double)inl / (double)onl;
  }
  else {
    sscale = ui.GetDouble ("SSCALE");
    lscale = ui.GetDouble ("LSCALE");
    ons = (int)ceil ((double)ins/sscale);
    onl = (int)ceil ((double)inl/lscale);
  }

  if (ons > ins || onl > inl) {
    string msg = "Number of output samples/lines must be less than or equal";
    msg = msg + " to the input samples/lines.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  //  Allocate output file
  Cube *ocube = NULL;
  try {
    ocube = p.SetOutputCube ("TO",ons,onl,inb);
    // Our processing routine only needs 1
    // the original set was for info about the cube only
    p.ClearInputCubes();
  } catch (iException &e) {
    // If there is a problem, catch it and close the cube so it isn't open next time around
    cube->Close();
    throw e;
  }

  //  Create all necessary buffers
  in = new LineManager (*cube);

  // Start the processing
  line = 1.0;
  iline = 1;
  sb = 0;
  if (alg == "AVERAGE") p.StartProcess(average);
  if (alg == "NEAREST") p.StartProcess(nearest);

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword ("InputLines", inl);
  results += PvlKeyword ("InputSamples", ins);
  results += PvlKeyword ("StartingLine", "1");
  results += PvlKeyword ("StartingSample", "1");
  results += PvlKeyword ("EndingLine", inl);
  results += PvlKeyword ("EndingSample", ins);
  results += PvlKeyword ("LineIncrement", lscale);
  results += PvlKeyword ("SampleIncrement", sscale);
  results += PvlKeyword ("OutputLines", onl);
  results += PvlKeyword ("OutputSamples", ons);
 
  // Update the Mapping, Instrument, and AlphaCube groups in the output
  // cube label
  SubArea s;
  s.SetSubArea(inl,ins,1,1,inl,ins,lscale,sscale);
  s.UpdateLabel(cube,ocube,results);
 
  // Cleanup
  p.EndProcess();
  delete in;
  cube->Close();

  // Write the results to the log
  Application::Log(results);
}

// Line processing routine for averaging algorithm
void average (Buffer &out) {
  static double *sinctab;
  static double *sum;
  static double *npts;
  static double *sum2;
  static double *npts2;

  double rline = (double)out.Line() * lscale;

  if (out.Line() == 1 && out.Band() == 1) {
    sinctab = new double[ons];
    sum = new double[ons];
    npts = new double[ons];
    sum2 = new double[ons];
    npts2 = new double[ons];

    //  Fill sinctab and Initialize buffers for first band
    for (int osamp=0; osamp<ons; osamp++) {
      sinctab[osamp] = ((double)osamp+1.) * sscale;
      sum[osamp] = 0.0;
      npts[osamp] = 0.0;
      sum2[osamp] = 0.0;
      npts2[osamp] = 0.0;
    }
    sinctab[ons-1] = ins;
  }

  while (iline <= rline) {
    if ((int)iline <= inl) {
      in->SetLine(iline,(iString::ToInteger(bands[sb])));
      cube->Read(*in);
    }
    int isamp = 1;
    for (int osamp=0; osamp<out.size(); osamp++) {
      while ((double)isamp <= sinctab[osamp]) {
        // If Pixel is valid add it to sum
        if (IsValidPixel((*in)[isamp-1])) {
          sum[osamp] += (*in)[isamp-1];
          npts[osamp] += 1.0;
        }
        isamp++;
      }

      double sdel = (double) isamp - sinctab[osamp];
      if (isamp > ins) continue;

      if (IsValidPixel( (*in)[isamp-1])) {
        sum[osamp] += (*in)[isamp-1] * (1.0 - sdel);
        npts[osamp] += (1.0 - sdel);
        if (osamp+1 < ons) {
          sum[osamp+1] += (*in)[isamp-1] * sdel;
          npts[osamp+1] += sdel;
        }
      }
      isamp++;
    }
    iline++;
  }

  if (iline <= inl) {
    in->SetLine(iline,(iString::ToInteger(bands[sb])));
    cube->Read(*in);
  }
  double ldel = (double)iline - rline;
  double ldel2 = 1.0 - ldel;
  int isamp = 1;
  for (int osamp=0; osamp<ons; osamp++) {
    while (isamp <= sinctab[osamp]) {
      if (IsValidPixel( (*in)[isamp-1])) {
        sum[osamp] += (*in)[isamp-1] * ldel2;
        npts[osamp] += ldel2;
        sum2[osamp] += (*in)[isamp-1] * ldel;
        npts2[osamp] += ldel;
      }
      isamp++;
    }

    double sdel = (double) isamp - sinctab[osamp];
    if (isamp > ins) continue;
    if (IsValidPixel( (*in)[isamp-1])) {
      sum[osamp] += (*in)[isamp-1] * (1.0 - sdel) * ldel2;
      npts[osamp] += (1.0 - sdel) * ldel2;
      if (osamp+1 < ons) {
        sum[osamp+1] += (*in)[isamp-1] * sdel * ldel2;
        npts[osamp+1] += sdel * ldel2;
      }
      sum2[osamp] += (*in)[isamp-1] * (1.0 - sdel) * ldel;
      npts2[osamp] += (1.0 - sdel) * ldel;
      if (osamp+1 < ons) {
        sum2[osamp+1] += (*in)[isamp-1] * sdel * ldel;
        npts2[osamp+1] += sdel * ldel;
      }
    }
    isamp++;
  }

  if (iline < inl) iline++;

  double npix = sscale * lscale;
  for (int osamp=0; osamp<ons; osamp++) {
    if (npts[osamp] > npix * vper ) {
      out[osamp] = sum[osamp] / npts[osamp];
    }
    else {
      if(replaceMode == "NEAREST") {
        out[osamp] = (*in)[(int)(sinctab[osamp]+0.5) - 1];
      }else{
        out[osamp] = Isis::Null;
      }
    }
    sum[osamp] = sum2[osamp];
    npts[osamp] = npts2[osamp];
    sum2[osamp] = 0.0;
    npts2[osamp] = 0.0;
  }

  if (out.Line() == onl && out.Band() != inb) {
    sb++;
    iline = 1;
    for (int osamp=0; osamp<ons; osamp++) {
      sum[osamp] = 0.0;
      npts[osamp] = 0.0;
      sum2[osamp] = 0.0;
      npts2[osamp] = 0.0;
    }
  }

  if (out.Line() == onl && out.Band() == inb) {
    delete [] sinctab;
    delete [] sum;
    delete [] npts;
    delete [] sum2;
    delete [] npts2;
  }
}

// Line processing routine for nearest-neighbor
void nearest (Buffer &out) {
  int readLine = (int)(line + 0.5);
  in->SetLine(readLine,(iString::ToInteger(bands[sb])));
  cube->Read(*in);

  //  Scale down buffer
  for (int osamp=0; osamp<ons; osamp++) {
    out[osamp] = (*in)[(int)((double)osamp*sscale)];
  }

  if (out.Line() == onl) {
    sb++;
    line = 1.0;
  }
  else {
    line += lscale;
  }
}
