/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include "BufferManager.h"
#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  cout << "Isis::BufferManager Unit Test" << endl << endl;

  Isis::BufferManager bm(6, 4, 2, 3, 2, 1, Isis::Real);

  for(bm.begin(); !bm.end(); bm++) {
    cout << "Position:  " << bm.Sample() << " "
         << bm.Line() << " "
         << bm.Band() << endl;
  }
  cout << endl;

  Isis::BufferManager bm2(4, 3, 2, 1, 1, 1, Isis::Real);

  bm2.begin();
  do {
    cout << "Position:  " << bm2.Sample() << " "
         << bm2.Line() << " "
         << bm2.Band() << endl;
  }
  while(bm2.next());
  cout << endl;

  Isis::BufferManager bm3(5, 1, 1, 2, 1, 1, Isis::Real);

  bm3.begin();
  do {
    cout << "Position:  " << bm3.Sample() << " "
         << bm3.Line() << " "
         << bm3.Band() << endl;
  }
  while(bm3.next());
  cout << endl;

  return 0;
}
