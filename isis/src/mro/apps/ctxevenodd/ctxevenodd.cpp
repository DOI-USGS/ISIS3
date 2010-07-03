//
//    ctxevenodd.cpp
//   
//    Remove even/odd striping from a CTX cube
//
//    outputCube = inputCube + {a correction offset to valid pixels}
//
//    calc average dn of all valid pixels located in either even or odd columns.
//    correction offset is 1/2 the difference between the even and odd average.
//    subtract the offset from pixels in even columns
//    add      the offset to   pixels in odd  columns
//

#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

void getStats (Buffer &in);
int evenCount, oddCount;
double evenSum, oddSum;

void applyCorrectionOffset (Buffer &in,Buffer &out);
double correctionOffset;


void IsisMain() {
  ProcessByLine p;
  p.SetInputCube("FROM");
  
  // Make sure we have a ctx cube and it has SpatialSumming of 1
  UserInterface &ui = Application::GetUserInterface();
  Isis::Pvl lab(ui.GetFilename("FROM"));
  Isis::PvlGroup &inst =
             lab.FindGroup("Instrument",Pvl::Traverse);

  std::string instId = inst["InstrumentId"];
  if (instId != "CTX") {
    string msg = "This is not a CTX image.  Ctxcevenodd requires a CTX image.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
  int sum = inst["SpatialSumming"];
  if (sum != 1) {
    string msg = "CTX images do not have even/odd noise problems";
    msg += " if the SpatialSumming is greater than one.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Get even and odd column statistics
  evenCount = 0;
  evenSum   = 0.0;
  oddCount  = 0;
  oddSum    = 0.0;
  Progress *progress = p.Progress();
  progress->SetText("Retrieving CTX Image Stats");
  p.StartProcess(getStats);
  p.EndProcess();
  
  // compute the correction offset
  //
  //  1/2 the difference between the even column average
  //  and the odd column average of valid pixel dn values.
  //
  // throw err if, pixel counts could result in division by zero
  if ((oddCount == 0) || (evenCount == 0)) {
    string msg = "Couldn't compute column averages";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  //  from mocevenodd
  correctionOffset = ((evenSum / evenCount) - (oddSum / oddCount)) / 2.0;


  // output cube - apply offset
  progress->SetText("Writing De-striped CTX Image");
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");
  p.StartProcess(applyCorrectionOffset);
  p.EndProcess();
}


void getStats (Buffer &in) {
  
  // ProcessByLine
  //
  // Count and sum all valid pixel values
  // in even and odd image columns.
  
  bool odd = true;
  
  for (int i=0; i<in.size(); i++) {
    if (IsValidPixel(in[i])) {
      if (odd) {
        oddSum += in[i];
        oddCount++;
      }
      else {
        evenSum += in[i];
        evenCount++;
      }
    }
    odd = !odd;
  }
}


void applyCorrectionOffset (Buffer &in, Buffer &out) {
  
  // ProcessByLine
  //
  //  add      'correction Offset' to   valid pixels in odd  columns
  //  subtract 'correction Offset' from valid pixels in even columns

  bool odd = true;
  
  for (int i=0; i<in.size(); i++) {
    if (IsValidPixel(in[i])) {
      if (odd) {
        out[i] = in[i] + correctionOffset;
      }
      else {
        out[i] = in[i] - correctionOffset;
      }
    }
    else {
      out[i] = in[i];
    }
    odd = !odd;
  }
}

