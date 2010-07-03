#ifndef WarpTransform_h
#define WarpTransform_h

#include <vector>
#include "Transform.h"
#include "BasisFunction.h"
#include "LeastSquares.h"
#include "PvlGroup.h"

namespace Isis {
class WarpTransform : public Transform {
    public:
      WarpTransform(Isis::BasisFunction &basisLine, 
           Isis::BasisFunction &basisSamp, bool weighted,
           std::vector<double> &inputLine, std::vector<double> &inputSample,
           std::vector<double> &outputLine, std::vector<double> &outputSample,
           int inputLines, int inputSamples, int outputLines, int outputSamples);
      ~WarpTransform();
  
      // Implementations for parent's pure virtual members
      bool Xform (double &inSample, double &inLine,
                      const double outSample, const double outLine);
      int OutputSamples () const { return p_outputSamples; };
      int OutputLines () const { return p_outputLines; };
      Isis::PvlGroup Residuals();
  
    private:
      int p_outputSamples;
      int p_outputLines;
      Isis::LeastSquares *p_lsqInputLine;
      Isis::LeastSquares *p_lsqInputSamp;
  
      std::vector<double> p_outputLine;
      std::vector<double> p_outputSample;
      bool p_weighted;
  };

}
#endif


