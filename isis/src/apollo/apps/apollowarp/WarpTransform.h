#ifndef WarpTransform_h
#define WarpTransform_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "Transform.h"
#include "BasisFunction.h"
#include "LeastSquares.h"
#include "PvlGroup.h"

namespace Isis {

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
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
