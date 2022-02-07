/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

//
//    mocevenodd.cpp
//
//    Remove even/odd striping from a MOC cube
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
#include "MocLabels.h"
#include "IException.h"

using namespace std;
using namespace Isis;

void getStats(Buffer &in);
int evenCount, oddCount;
double evenSum, oddSum;

void applyCorrectionOffset(Buffer &in, Buffer &out);
double correctionOffset;


void IsisMain() {
  // Make sure we have a moc cube
  ProcessByLine p;
  p.SetInputCube("FROM");
  MocLabels moc(Application::GetUserInterface().GetCubeName("FROM"));

  // It must have crosstrack summing of 1
  if(moc.CrosstrackSumming() != 1) {
    string msg = "MOC images do not have even/odd noise problems";
    msg += " if the crosstrack summing mode greater than one";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Get even and odd column statistics
  evenCount = 0;
  evenSum   = 0.0;
  oddCount  = 0;
  oddSum    = 0.0;
  Progress *progress = p.Progress();
  progress->SetText("Retrieving MOC Image Stats");
  p.StartProcess(getStats);
  p.EndProcess();

  // compute the correction offset
  //
  //  1/2 the difference between the even column average
  //  and the odd column average of valid pixel dn values.
  //
  // throw err if, pixel counts could result in division by zero
  if((oddCount == 0) || (evenCount == 0)) {
    string msg = "Couldn't compute column averages";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  correctionOffset = ((evenSum / evenCount) - (oddSum / oddCount)) / 2.0;

  // output cube - apply offset
  progress->SetText("Writing De-striped MOC Image");
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  p.StartProcess(applyCorrectionOffset);
  p.EndProcess();
}


void getStats(Buffer &in) {

  // ProcessByLine
  //
  // Count and sum all valid pixel values
  // in even and odd image columns.

  bool odd = true;

  for(int i = 0; i < in.size(); i++) {
    if(IsValidPixel(in[i])) {
      if(odd) {
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


void applyCorrectionOffset(Buffer &in, Buffer &out) {

  // ProcessByLine
  //
  //  add      'correction Offset' to   valid pixels in odd  columns
  //  subtract 'correction Offset' from valid pixels in even columns

  bool odd = true;

  for(int i = 0; i < in.size(); i++) {
    if(IsValidPixel(in[i])) {
      if(odd) {
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
