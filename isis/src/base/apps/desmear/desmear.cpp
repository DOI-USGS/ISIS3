#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void desmear (Buffer &in, Buffer &out);
double smearScale;  //Declare global variable

void IsisMain() {

  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output cubes
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube ("TO");

  // Get exposure duration and tranfer time
  // Override the lable values if the user entered a value
  double expTime,xferTime; 
  UserInterface &ui = Application::GetUserInterface();
  if (ui.WasEntered ("DURATION")) {
    expTime = ui.GetDouble ("DURATION");
  } 
  else {
    PvlGroup grp = icube->GetGroup("ISIS_INSTRUMENT");
    expTime = grp["EXPOSURE_DURATION"];
  }

  if (ui.WasEntered ("TRANSFER")) {
    xferTime = ui.GetDouble ("TRANSFER");
  } 
  else {
    PvlGroup grp = icube->GetGroup("ISIS_INSTRUMENT");
    xferTime = grp["TRANSFER_TIME"];
  }

  // Calculate the smear scale
  smearScale = xferTime / expTime / icube->Lines();

  // Start the processing
  p.StartProcess(desmear);
  p.EndProcess();
}

// Line processing routine
void desmear  (Buffer &in, Buffer &out) {
  //make an array that is size of number of samples and fill with zeros.
  static vector<double> smear;
  if (in.Line() == 1) {
    smear.resize(in.size());
    for (int i=0; i<in.size(); i++) {
      smear[i] = 0.0;
    }
  }

  // Loop and apply smear correction to each sample.
  for (int samp=0; samp<in.size(); samp++) {
    if (IsValidPixel(in[samp])) {
      if (in.Line() == 1) {
        //if first line: calculate smear for next line and output is input.
        smear[samp] = in[samp] * smearScale;
        out[samp] = in[samp];
      }
      else {
        smear[samp] = in[samp] * smearScale + 
                      smear[samp] * (1.0 - smearScale);

        out[samp] = in[samp] - (smear[samp]);
        if (out[samp] <= 0.0) {
          out[samp] = Isis::Null;
        }
      }
    }
    else {
      //if invalid pixel then output the same.
      out[samp] = in[samp];
    }
  }
}
