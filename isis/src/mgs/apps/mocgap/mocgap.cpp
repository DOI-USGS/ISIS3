#include "Isis.h"
#include "ProcessByLine.h"
#include "Statistics.h"
#include "SpecialPixel.h"
#include <vector>

using namespace std;
using namespace Isis;

double maxsd;

void fixgap(Buffer &in, Buffer &out);

void IsisMain() {
  //We will be processing by line
  ProcessByLine p;

  //Set up the input and output cubes, and set maximum standard deviation
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  UserInterface &ui = Application::GetUserInterface();
  maxsd = ui.GetDouble("MAXSD");

  //Begin processing
  p.StartProcess(fixgap);
  p.EndProcess();

}

void fixgap (Buffer &in, Buffer &out) {
  //First, copy the entire line over
  for (int i=0; i<in.size(); i++) {
    out[i] = in[i];
  }
  
  //Place the five pixels preceeding sample 371 into a vector
  vector<double> tempData;
  for (int j=365; j<=369; ++j) {
    if (!IsSpecial(in[j])) {
      tempData.push_back(in[j]);
    }
  }
  
  //Move the values from the vector into an array of doubles.
  //This step and the previous one are included because with the vector, we
  //can acount for special pixels, otherwise we might end up with trailing zeros
  //in the array, skewing the data. Also, Statistics requires a double array.
  double data[tempData.size()];
  for (int i=0; i<(int)tempData.size(); ++i) {
    data[i]=tempData[i];
  }
  Statistics stats;
  stats.AddData(data,tempData.size());
  //calulate average and standard deviation
  double avg = stats.Average();
  double sd = stats.StandardDeviation();

  //Determine the boundaries of the valid pixel value range
  double upperbound = avg + sd / maxsd;
  double lowerbound = avg - sd / maxsd;
  //If the pixel at 371 is valid, there's no need to replace it
  if ( in[370] <= upperbound && in[370] >= lowerbound ) {
    return;
  }
  //Otherwise, we need to see if samples 372 and 370 are valid, and we'll 
  //replace 371 with the average of these two.
  else if (in[369] >= lowerbound && in[369] <= upperbound) {
    if (in[371] <= upperbound && in[371] >= lowerbound) {
      out[370] = (in[369] + in[371])/2.0;
    }
    //If 372 isn't valid, we look out one farther and replace 371 and 372,
    //using the average of 370 and 373, weighting the adjacent pixel higher
    else if (in[372] <= upperbound && in[372] >= lowerbound) {
      out[370] = (2.0 * in[369])/3.0 + in[372]/3.0;
      out[371] = in[369]/3.0 + (2.0 * in[372])/3.0;
    }
  }
}
