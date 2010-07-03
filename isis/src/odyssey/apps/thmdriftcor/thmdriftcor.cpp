#include "Isis.h"

#include <string>

#include "ProcessByLine.h"
#include "UserInterface.h"
#include "QuickFilter.h"
#include "Statistics.h"

using namespace std;
using namespace Isis;

vector<double> lineAverage;
void getStats(Buffer &in);
void driftcor(Buffer &in, Buffer &out);

void IsisMain () {

  lineAverage.clear();
  ProcessByLine p;
  p.SetInputCube("ATM",Isis::OneBand);
  p.StartProcess(getStats);
  p.EndProcess();

  QuickFilter filt(lineAverage.size(),257,1);
  filt.AddLine(&lineAverage[0]);

  for (int i=0; i<(int)lineAverage.size(); i++) {
    lineAverage[i] = filt.Average(i);
  }

  double base = lineAverage[lineAverage.size()-1];
  for (int i=0; i<(int)lineAverage.size(); i++) {
    lineAverage[i] = lineAverage[i] - base;
    lineAverage[i] = lineAverage[i] * 12854.7 / 11960.5;
  }

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  p.StartProcess(driftcor);
  p.EndProcess();

  return;
}

void getStats(Buffer &in) {
  Statistics stats;
  stats.AddData(in.DoubleBuffer(),in.size());
  if( stats.Average() == Isis::Null ) {
    lineAverage.push_back( 0.0 );
  }
  else {
    lineAverage.push_back(stats.Average());
  }
}

void driftcor(Buffer &in, Buffer &out) {
  for (int i=0; i<in.size(); i++) {
    out[i] = in[i] - lineAverage[in.Line()-1];
  }
}
