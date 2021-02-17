/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>
#include "GaussianStretch.h"
#include "Histogram.h"
#include "ImageHistogram.h"
#include "IException.h"
#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  Isis::Histogram *hist = new Isis::ImageHistogram(0, 99, 100);

  cout << setprecision(14);

  int numData = 100;
  double data[numData];
  for(int i = 0; i < numData; i++) {
    data[i] = i;
  }

  hist->AddData(data, numData);
  Isis::GaussianStretch g(*hist, 49.5, 14.0);
  for(int i = 0; i < numData; i++) {
    cout << data[i] << "    " << g.Map(data[i]) << endl;
  }
}
