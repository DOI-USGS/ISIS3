#ifndef FourierTransform_h
#define FourierTransform_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <complex>
#include <vector>
#include "Constants.h"

namespace Isis {
  /**
   * @brief Fourier Transform class
   *
   *     This class is used to apply a Fourier transform to a vector of complex
   * data as well as the inverse Fourier transform. Applying the Fourier
   * transform on data in the spatial domain will convert it to data in the
   * Fourier (or frequency) domain. The inverse transform takes data
   * from the frequency domain to the spatial.
   *
   * If you would like to see FourierTransform being used
   *         in implementation, see fft.cpp or ifft.cpp.
   *
   * @ingroup Math and Statistics
   *
   * @author 2005-11-28 Jacob Danton
   *
   * @internal
   */
  class FourierTransform {
    public:
      FourierTransform();
      ~FourierTransform();
      std::vector< std::complex<double> > Transform(std::vector< std::complex<double> > input);
      std::vector< std::complex<double> > Inverse(std::vector< std::complex<double> > input);
      bool IsPowerOfTwo(int n);
      int lg(int n);
      int BitReverse(int n, int x);
      int NextPowerOfTwo(int n);
  };
}

#endif
