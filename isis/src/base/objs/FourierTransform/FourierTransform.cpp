/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FourierTransform.h"

using namespace std;

namespace Isis {
  //! Constructs the FourierTransform object.
  FourierTransform::FourierTransform() {};

  //! Destroys the FourierTransform object.
  FourierTransform::~FourierTransform() {};

  /**
   * Applies the Fourier transform on the input data
   * and returns the result.
   *
   * @param input The data to be transformed.
   *
   * @return vector
   */
  std::vector< std::complex<double> >
  FourierTransform::Transform(std::vector< std::complex<double> > input) {
    // data length must be a power of two
    // any extra space is filled with zeroes
    int n = NextPowerOfTwo(input.size());
    vector< std::complex<double> > output(n);
    input.resize(n);

    // rearrange the data to fit the iterative algorithm
    // which will apply the transform from the bottom up
    for(int i = 0; i < n; i++) {
      if(output[i] == 0.0) {
        int j = BitReverse(n, i);
        output[i] = input[j];
        output[j] = input[i];
      }
    }

    // do the iterative fft calculation by first combining
    // subarrays of length 2, then 4, 8, etc.
    for(int m = 1; m < n; m *= 2) {
      complex<double> Wm(polar(1.0, -1.0 * PI / m)); // Wm = e^(-PI/m *i)
      for(int k = 0; k < n; k += 2 * m) {
        // W = Wm^j, the roots of unity for x^m=1
        complex<double> W(1.0);
        for(int j = 0; j < m; j++) {
          complex<double> t = W * output[k+j+m]; // the "twiddle" factor
          complex<double> u = output[k+j];
          output[k+j] = u + t; // a[k+j]+Wm^j*a[k+j+m]
          output[k+j+m] = u - t; // a[k+j]+Wm^(j+m)*[k+j+m] = a[k+j]-Wm^j*[k+j+m]
          W = W * Wm;
        }
      }
    }

    return output;
  }


  /**
   * Applies the inverse Fourier transform on the input data
   * and returns the result.
   *
   * @param input The data to be transformed.
   *
   * @return vector
   */
  std::vector< std::complex<double> >
  FourierTransform::Inverse(std::vector< std::complex<double> > input) {
    // Inverse(input) = 1/n*conj(Transform(conj(input)))
    int n = input.size();
    std::vector< std::complex<double> > temp(n);
    for(int i = 0; i < n; i++) {
      temp[i] = conj(input[i]);
    }

    std::vector< std::complex<double> > output = Transform(temp);

    for(int i = 0; i < n; i++) {
      output[i] = conj(output[i]) / ((double)n);
    }

    return output;
  }

  /**
   * Checks to see if the input integer is a power of two
   *
   * @param n The input integer
   *
   * @return bool - Returns true if the input is a power of two and false if it
   *                is not
   */
  bool FourierTransform:: IsPowerOfTwo(int n) {
    while(n > 1) {
      if(n % 2 == 1) return false;
      n /= 2;
    }

    return true;
  }

  /**
   * This function returns the floor of log2(n)
   *
   * @param n The input integer
   *
   * @return int - the floor of log2(n)
   */
  int FourierTransform:: lg(int n) {
    int k = 0;
    while(n > 1) {
      n = n / 2;
      k++;
    }
    return k;
  }

  /**
   * Reverses the binary representation of the input integer
   * in the number of bits specified.
   * bitReverse(n,x) =
   * n/2*(2^-a1 + 2^-a2 + ... + 2^-ak) where x = 2^a1 + 2^a2 + ... + 2^ak
   *
   * @param n The number of bits
   *
   * @param x The input integer
   *
   * @return int - The reverse of the binary representation of the input integer
   */
  int FourierTransform::BitReverse(int n, int x) {
    double ans = 0.0;
    double a = 1.0;
    while(x > 0) {
      if(x % 2 == 1) {
        ans += a;
        x -= 1;
      }
      x = x / 2;
      a = a / 2.0;
    }

    return(int)(ans * n / 2);
  }

  /**
   * This function returns the next power of two greater than or equal to n.
   * If n is a power of two, it returns n.
   *
   * @param n The number of bits
   *
   * @return int - the next power of two greater than or equal to n
   */
  int FourierTransform::NextPowerOfTwo(int n) {
    if(IsPowerOfTwo(n)) return n;
    return(int)pow(2.0, lg(n) + 1);
  }
}
