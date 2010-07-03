#include <iostream>
#include <iomanip>
#include "GaussianDistribution.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
  Isis::GaussianDistribution g;

  cout << setprecision(11);

  cout << "Cumulative Distribution" << endl;
  for (int i=-30; i<=30; i +=5) {
    double x = i/10.0;
    cout << x << "    " << g.CumulativeDistribution(x) << "%"<< endl;
  }

  cout << "Inverse Cumulative Distribution" << endl;
  for (int i=0; i<=10; i++) {
    double p = 0.1*i;
    cout << (10*i) << "%    " << g.InverseCumulativeDistribution(p*100.0) << endl;
  }
}
