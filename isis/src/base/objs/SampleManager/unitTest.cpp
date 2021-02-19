/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <iostream>
#include <stdio.h>

#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "SampleManager.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  QString fname = "$ISISTESTDATA/isis/src/base/unitTestData/isisTruth.cub";
  Isis::Cube cube;
  cube.open(fname);

  Isis::SampleManager sample(cube);
  cout << "Buffer Size:  " <<
       sample.SampleDimension() << " " <<
       sample.LineDimension() << " " <<
       sample.BandDimension() << endl;
  cout << endl;

  for(sample.begin(); !sample.end(); sample++) {
    cout << "  Current sample, line, band is:  "
         << sample.Sample() << " "
         << sample.Line() << " "
         << sample.Band() << endl;
  }
  cout << endl;

  Isis::SampleManager sampleReverse(cube, true);
  cout << "Buffer Size:  " <<
       sampleReverse.SampleDimension() << " " <<
       sampleReverse.LineDimension() << " " <<
       sampleReverse.BandDimension() << endl;
  cout << endl;

  for(sampleReverse.begin(); !sampleReverse.end(); sampleReverse++) {
    cout << "  Current sample, line, band is:  "
         << sampleReverse.Sample() << " "
         << sampleReverse.Line() << " "
         << sampleReverse.Band() << endl;
  }
  cout << endl;

  sample.SetSample(50);
  cout << "  Current sample, line, band is:  "
       << sample.Sample() << " "
       << sample.Line() << " "
       << sample.Band() << endl;
  cout << endl;

  sampleReverse.SetSample(50);
  cout << "  Current sample, line, band is:  "
       << sampleReverse.Sample() << " "
       << sampleReverse.Line() << " "
       << sampleReverse.Band() << endl;
  cout << endl;

  try {
    cout << "Testing errors ... " << endl;
    sample.SetSample(0, 0);
  }
  catch(Isis::IException &e) {
    e.print();
    cout << endl;
  }

  try {
    cout << "Testing errors ... " << endl;
    sample.SetSample(1, 0);
  }
  catch(Isis::IException &e) {
    e.print();
    cout << endl;
  }

  cube.close();
}
