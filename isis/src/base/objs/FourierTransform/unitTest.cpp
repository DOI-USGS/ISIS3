#include <iostream>
#include <iomanip>
#include "FourierTransform.h"
#include "Preference.h"

using namespace std;

std::complex<double> Round(std::complex<double> n);

int main(int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
    Isis::FourierTransform fft;
    int n = 13;
    vector< std::complex<double> > original(n);
    for (int i=0; i<n; i++)
    {
      original[i] = std::complex<double>(i, n-i);
    }

    vector< std::complex<double> > transformed = fft.Transform(original);

    vector< std::complex<double> > inverted = fft.Inverse(transformed);

    original.resize(inverted.size());

    cout << "Original    Transformed    Inverted" << endl;

    for (unsigned int i=0; i<transformed.size(); i++)
    {
      cout << Round(original[i]) << " " << Round(transformed[i])
           << " " << Round(inverted[i]) << endl;
    }
}

// To fix round off error and differences between architecture
std::complex<double> Round(std::complex<double> n) {
    double CUTOFF = std::pow(10.0, -14.0);
    double real = n.real();
    double imag = n.imag();
    if (real < CUTOFF) real = 0;
    if (imag < CUTOFF) imag = 0;
    return std::complex<double>(real, imag);
}
