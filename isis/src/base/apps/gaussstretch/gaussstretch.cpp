#include "ProcessByLine.h"
#include "Statistics.h"
#include "GaussianStretch.h"
#include "gaussstretch.h"

using namespace std;
using namespace Isis;

namespace Isis {
  void gauss(Buffer &in, Buffer &out);

  vector<GaussianStretch *> stretch;

  void gaussstretch(UserInterface &ui) {
    Cube icube;
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube.setVirtualBands(inAtt.bands());
    }
    icube.open(ui.GetCubeName("FROM"));
    gaussstretch(&icube, ui);
  }

  void gaussstretch(Cube *icube, UserInterface &ui) {
    ProcessByLine p;
    p.SetInputCube(icube);
    QString outputFileName = ui.GetCubeName("TO");
    CubeAttributeOutput outputAttributes= ui.GetOutputAttribute("TO");
    p.SetOutputCube(outputFileName, outputAttributes, 
                    icube->sampleCount(), icube->lineCount(), icube->bandCount());
    double gsigma = ui.GetDouble("GSIGMA");

    for(int i = 0; i < icube->bandCount(); i++) {
      Histogram *hist = icube->histogram(i + 1);
      double mean = (hist->Maximum() + hist->Minimum()) / 2.0;
      double stdev = (hist->Maximum() - hist->Minimum()) / (2.0 * gsigma);
      stretch.push_back(new GaussianStretch(*hist, mean, stdev));
    }

    p.StartProcess(gauss);
    for(int i = icube->bandCount()-1; i >= 0 ; i--) {
      delete stretch[i];
      stretch.pop_back();
    }
    p.EndProcess();

    stretch.clear();
  }

  // Processing routine for the pca with one input cube
  void gauss(Buffer &in, Buffer &out) {
    for(int i = 0; i < in.size(); i++) {
      if(IsSpecial(in[i])) out[i] = in[i];
      out[i] = stretch[in.Band(i)-1]->Map(in[i]);
    }
  }
}
