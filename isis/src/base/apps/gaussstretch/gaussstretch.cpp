#include "Isis.h"
#include "ProcessByLine.h"
#include "Statistics.h"
#include "GaussianStretch.h"

using namespace std;
using namespace Isis;

void gauss (Buffer &in, Buffer &out);

vector<GaussianStretch *> stretch;

void IsisMain()
{
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  double gsigma = Isis::Application::GetUserInterface().GetDouble("GSIGMA");

  for (int i=0; i< icube->Bands(); i++) {
    Histogram hist = *(icube->Histogram(i+1));
    double mean = (hist.Maximum() + hist.Minimum())/2.0;
    double stdev = (hist.Maximum() - hist.Minimum())/(2.0*gsigma);
    stretch.push_back( new GaussianStretch( hist, mean, stdev) );
  }

  p.StartProcess(gauss);
  for (int i=0; i< icube->Bands(); i++) delete stretch[i];
  stretch.clear();
  p.EndProcess();
}

// Processing routine for the pca with one input cube
void gauss (Buffer &in, Buffer &out){
  for (int i=0; i<in.size();i++) {
    if (IsSpecial(in[i])) out[i] = in[i];
    out[i] = stretch[in.Band(i)-1]->Map(in[i]);
  }
}

