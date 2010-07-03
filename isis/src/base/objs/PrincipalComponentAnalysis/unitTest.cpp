#include <iostream>
#include "PrincipalComponentAnalysis.h"
#include "Preference.h"

using namespace std;

double Round(double n);

int main(int argc, char *argv[])
{
  Isis::Preference::Preferences(true);

  int n = 10;
  int k= 3;

  Isis::PrincipalComponentAnalysis pca(k);
  double original[k*n];
  for (int i=0; i<k; i++) {
    for (int j=0; j<n; j++) {
      original[i*n+j] = (i+j)%10;
    }
  }

  pca.AddData(original, n);
  pca.ComputeTransform();

  double components[k*n];
  double inverted[k*n];
  for (int i=0; i<n; i++) {
    TNT::Array2D<double> x(1, k);
    for (int j=0; j<k; j++) {
      x[0][j] = original[i+n*j];
    }

    TNT::Array2D<double> y = pca.Transform(x);
    for (int j=0; j<k; j++) {
      components[i+n*j] = y[0][j];
    }

    TNT::Array2D<double> z = pca.Inverse(y);
    for (int j=0; j<k; j++) {
      inverted[i+n*j] = z[0][j];
    }
  }

  cout << "  Original        Principal Components        Inverted" << endl;

  for (int i=0; i<n; i++) {
    cout << "    ";
    for (int j=0; j<k; j++) {
      cout << Round( original[i+n*j] ) << " ";
    }
    cout << "  ->  ";
    for (int j=0; j<k; j++) {
      cout << Round( components[i+n*j] ) << " ";
    }
    cout << "  ->  ";
    for (int j=0; j<k; j++) {
      cout << Round( inverted[i+n*j] ) << " ";
    }
    cout << endl;
  }
}

// To fix round off error and differences between architecture
double Round(double n) {
  double CUTOFF = std::pow(10.0, -14.0);
  if (abs(n) < CUTOFF) return 0;
  return n;
}
