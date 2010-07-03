#include "Isis.h"

#include <vector>
#include <algorithm>
#include <stdio.h>

#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

enum SpecPix {
  NULLP,
  LRS,
  HRS,
  LIS,
  HIS,
  NONE
};

//  Define class for checking the input ranges to make sure
//  there is no overlap.  If there is any overlap, exit with
//  user error.
struct spRange {
  double min;
  double max;
  SpecPix specPix;
};
vector <spRange> rngList;
int numRange;

// Line processing routine
void specpix (Buffer &in, Buffer &out);

bool Descending (const spRange &r1,const spRange &r2);
int nnull,nlis,nlrs,nhis,nhrs;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  nnull = nlis = nlrs = nhis = nhrs = 0;

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");

  spRange temp;
  // Read range values from user
  UserInterface &ui = Application::GetUserInterface();

  temp.min = 0;
  temp.max = 0;
  if (ui.WasEntered ("NULLMIN")) temp.min = ui.GetDouble ("NULLMIN");
  if (ui.WasEntered ("NULLMAX")) temp.max = ui.GetDouble ("NULLMAX");

  if (temp.min != 0 || temp.max != 0) {
    nnull = 0;
    temp.specPix = NULLP;
    rngList.push_back (temp);
    temp.min = 0;
    temp.max = 0;
    temp.specPix = NONE;
  }

  if (ui.WasEntered ("LRSMIN")) temp.min = ui.GetDouble ("LRSMIN");
  if (ui.WasEntered ("LRSMAX")) temp.max = ui.GetDouble ("LRSMAX");
  if (temp.min != 0 || temp.max != 0) {
    nlrs = 0;
    temp.specPix = LRS;
    rngList.push_back (temp);
    temp.min = 0;
    temp.max = 0;
    temp.specPix = NONE;
  }

  if (ui.WasEntered ("HRSMIN")) temp.min = ui.GetDouble ("HRSMIN");
  if (ui.WasEntered ("HRSMAX")) temp.max = ui.GetDouble ("HRSMAX");
  if (temp.min != 0 || temp.max != 0) {
    nhrs = 0;
    temp.specPix = HRS;
    rngList.push_back (temp);
    temp.min = 0;
    temp.max = 0;
    temp.specPix = NONE;
  }

  if (ui.WasEntered ("LISMIN")) temp.min = ui.GetDouble ("LISMIN");
  if (ui.WasEntered ("LISMAX")) temp.max = ui.GetDouble ("LISMAX");
  if (temp.min != 0 || temp.max != 0) {
    nlis = 0;
    temp.specPix = LIS;
    rngList.push_back (temp);
    temp.min = 0;
    temp.max = 0;
    temp.specPix = NONE;
  }

  if (ui.WasEntered ("HISMIN")) temp.min = ui.GetDouble ("HISMIN");
  if (ui.WasEntered ("HISMAX")) temp.max = ui.GetDouble ("HISMAX");
  if (temp.min != 0 || temp.max != 0) {
    nhis = 0;
    temp.specPix = HIS;
    rngList.push_back (temp);
    temp.min = 0;
    temp.max = 0;
    temp.specPix = NONE;
  }

  //  If more than one range was entered,
  //  make sure there is no overlap in ranges between differing special
  //  pixel values.  First sort on the min value in descending order.
  //  Then compare each min to the max in the next set of ranges.  If
  //  the min is less than the next max, there is overlap between those
  //  two sets of ranges.
  numRange = rngList.size();
  if (numRange > 1) {
    vector <spRange> sortList(numRange);
    copy (rngList.begin(), rngList.end(), sortList.begin());
    sort (sortList.begin(),sortList.end(),Descending);
    for (int i=0; i<numRange-1; i++) {
      if (sortList[i].min < sortList[i+1].max) {
        //  We have overlap
        string message = "Check the ranges entered for overlap between differing  ";
        message += "special pixels.  ";
        throw iException::Message(iException::User,message,_FILEINFO_);
      }
    }
    //  Copy sorted list back to original vector.
    copy (sortList.begin(), sortList.end(), rngList.begin());
  }

  // Start the processing
  p.StartProcess(specpix);
  p.EndProcess();

  // Erase vector, otherwise if specpix is re-run it adds new
  // ranges to the end of the last run.
  rngList.erase(rngList.begin(),rngList.end());

  //  Print out number of values changed
  PvlGroup results("Results");
  results.AddComment("The number and type of pixels created");
  results += PvlKeyword("Null",nnull);
  results += PvlKeyword("Lrs",nlrs);
  results += PvlKeyword("Lis",nlis);
  results += PvlKeyword("Hrs",nhrs);
  results += PvlKeyword("His",nhis);
  int total = nnull + nlrs + nhrs + nlis + nhis;
  results += PvlKeyword("Total",total);

  Application::Log(results);

}


//  Line processing routine
void specpix (Buffer &in,Buffer &out)
{	

  for (int i=0; i<in.size (); i++) {
    out[i] = in[i];
    for (int rng=0; rng<numRange; rng++) {
      switch (rngList[rng].specPix) {
        case NULLP:
          if (in[i] >= rngList[rng].min && in[i] <= rngList[rng].max) {
            out[i] = NULL8;
            nnull++;
          }
          break;

        case LRS:
          if (in[i] >= rngList[rng].min && in[i] <= rngList[rng].max) {
            out[i] = LOW_REPR_SAT8;
            nlrs++;
          }
          break;

        case HRS:
          if (in[i] >= rngList[rng].min && in[i] <= rngList[rng].max) {
            out[i] = HIGH_REPR_SAT8;
            nhrs++;
          }
          break;

        case LIS:
          if (in[i] >= rngList[rng].min && in[i] <= rngList[rng].max) {
            out[i] = LOW_INSTR_SAT8;
            nlis++;
          }
          break;

        case HIS:
          if (in[i] >= rngList[rng].min && in[i] <= rngList[rng].max) {
            out[i] = HIGH_INSTR_SAT8;
            nhis++;
          }
          break;

        case NONE:
          break;
      }
    }
  }
}

bool Descending (const spRange &r1,const spRange &r2)
{
  if (r1.min > r2.min) {
    return true;
  }
  else
    return false;
}
