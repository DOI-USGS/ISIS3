/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include "Buffer.h"
#include "Brick.h"
#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  cout << "Isis::Brick Unit Test" << endl << endl;

  Isis::Brick b(4, 3, 2, Isis::SignedInteger);

  cout << "SampleDimension:  " << b.SampleDimension() << endl;
  cout << "LineDimension:    " << b.LineDimension() << endl;
  cout << "BandDimension:    " << b.BandDimension() << endl;
  cout << "Size:             " << b.size() << endl << endl;

  b.Resize(9, 8, 7);

  cout << "SampleDimension:  " << b.SampleDimension() << endl;
  cout << "LineDimension:    " << b.LineDimension() << endl;
  cout << "BandDimension:    " << b.BandDimension() << endl;
  cout << "Size:             " << b.size() << endl << endl;

  b.SetBasePosition(3, 2, 1);
  cout << "Sample():         " << b.Sample() << endl;
  cout << "Line():           " << b.Line() << endl;
  cout << "Band():           " << b.Band() << endl << endl;

  b.SetBaseSample(5);
  b.SetBaseLine(6);
  b.SetBaseBand(7);

  cout << "Sample():         " << b.Sample() << endl;
  cout << "Line():           " << b.Line() << endl;
  cout << "Band():           " << b.Band() << endl << endl;

  return 0;
}
