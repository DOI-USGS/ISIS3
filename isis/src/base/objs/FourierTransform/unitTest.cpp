/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>
#include "FourierTransform.h"
#include "Preference.h"

using namespace std;

std::complex<double> Round(std::complex<double> n);

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  Isis::FourierTransform fft;
  int n = 13;
  vector< std::complex<double> > original(n);
  for(int i = 0; i < n; i++) {
    original[i] = std::complex<double>(i, n - i);
  }

  vector< std::complex<double> > transformed = fft.Transform(original);

  vector< std::complex<double> > inverted = fft.Inverse(transformed);

  original.resize(inverted.size());

  cout << "Original    Transformed    Inverted" << endl;

  for(unsigned int i = 0; i < transformed.size(); i++) {
    cout << Round(original[i]) << " " << Round(transformed[i])
         << " " << Round(inverted[i]) << endl;
  }
}

// To fix round off error and differences between architecture
std::complex<double> Round(std::complex<double> n) {
  double CUTOFF = std::pow(10.0, -14.0);
  double real = n.real();
  double imag = n.imag();
  if(real < CUTOFF) real = 0;
  if(imag < CUTOFF) imag = 0;
  return std::complex<double>(real, imag);
}
