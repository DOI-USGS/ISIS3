#include <cmath>
#include <iostream>

#include "WarpTransform.h"

namespace Isis {
  WarpTransform::WarpTransform(BasisFunction &basisLine, 
       BasisFunction &basisSamp, bool weighted, std::vector<double> &inputLine, 
       std::vector<double> &inputSample, std::vector<double> &outputLine,
       std::vector<double> &outputSample, int inputLines, int inputSamples, 
       int outputLines, int outputSamples) {
       
    p_outputLines = outputLines;
    p_outputSamples = outputSamples;
  
    // Create the equations for the control points using a least squares fit
    p_lsqInputSamp = new LeastSquares(basisSamp);
    p_lsqInputLine = new LeastSquares(basisLine);
    std::vector<double> known;
    known.resize(2);
    for (int i=0; i<(int)inputLine.size(); i++) {
      known[0] = outputLine[i];
      known[1] = outputSample[i];
      p_lsqInputLine->AddKnown(known,inputLine[i]);
      p_lsqInputSamp->AddKnown(known,inputSample[i]);
    }
  
    p_lsqInputLine->Solve();
    p_lsqInputSamp->Solve();
  
    p_weighted = weighted;
    if (weighted) {
      p_outputLine = outputLine;
      p_outputSample = outputSample;
    }
  }
  
  WarpTransform::~WarpTransform() {
    if (p_lsqInputLine != NULL) delete p_lsqInputLine;
    if (p_lsqInputSamp != NULL) delete p_lsqInputSamp;
  }
  
   
  // Convert the requested output samp/line to an input samp/line
  bool WarpTransform::Xform (double &inSample, double &inLine,
                        const double outSample, const double outLine) {
    if (p_weighted) {
      for (int i=0; i<(int) p_outputLine.size(); i++) {
        double dist = (outLine - p_outputLine[i]) *
                      (outLine - p_outputLine[i]) +
                      (outSample - p_outputSample[i]) *
                      (outSample - p_outputSample[i]);
        dist = sqrt(dist);
        double weight = 1.0;
        if (dist >= 0.001) weight = 1.0 / dist;
        p_lsqInputLine->Weight(i,weight);
        p_lsqInputSamp->Weight(i,weight);
      }
      p_lsqInputLine->Solve();
      p_lsqInputSamp->Solve();
    }
  
    static std::vector<double> vars;
    if (vars.size() != 2) vars.resize(2);
    vars[0] = outLine;
    vars[1] = outSample;
    inLine = p_lsqInputLine->Evaluate(vars);
    inSample = p_lsqInputSamp->Evaluate(vars);
    return true;
  }
  
  PvlGroup WarpTransform::Residuals () {
    PvlGroup errs("Residuals");
    for (int i=0; i<p_lsqInputLine->Knowns(); i++) {
      PvlKeyword p("POINT"+iString(i+1));
      p += p_lsqInputLine->Residual(i);
      p += p_lsqInputSamp->Residual(i);
      errs += p;
    }
    return errs;
  }
}
