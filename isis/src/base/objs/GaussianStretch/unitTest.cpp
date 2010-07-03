#include <iostream>
#include <iomanip>
#include "GaussianStretch.h"
#include "Histogram.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
  Isis::Histogram hist(0, 99, 100);

  cout << setprecision(14);

  int numData = 100;
  double data[numData];
  for (int i=0; i<numData; i++) {
    data[i] = i;
  }

  hist.AddData(data, numData);
  Isis::GaussianStretch g(hist,49.5,14.0);
  for (int i=0; i<numData; i++) {
    cout << data[i] << "    " << g.Map(data[i]) << endl;
  }
}
