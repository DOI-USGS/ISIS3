#ifndef VecFilter_h
#define VecFilter_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include <QString>

namespace Isis {
  /**
  * @brief This class is used to perform filter operations on vectors.
  *
  * This class performs boxcar filter operations on vectors. The boxcar
  * will be a one dimensional Nx1 boxcar where N is a positive odd
  * integer.
  *
  * For an example of how the VecFilter object is used in %Isis, see the
  * hicubenorm.cpp application.
  *
  * @ingroup Statistics
  *
  * @author 2009-03-13 Janet Barrett
  *
  * @internal
  *   @history 2011-02-08 Sharmila Prasad - Extended Highpass API for hicubenorm
  */
  class VecFilter {
    public:
      VecFilter();
      ~VecFilter();

      std::vector<double> LowPass(std::vector<double> invec, int boxsize);
      std::vector<double> HighPass(std::vector<double> invec1, std::vector<double> invec2);
      std::vector<double> HighPass(std::vector<double> pdInVector1, std::vector<double> pdInVector2, 
             std::vector<int> piValidPntsVector, int piMaxPoints, const QString & psMode="SUBTRACT");

    private:
  };
} // end namespace isis

#endif

