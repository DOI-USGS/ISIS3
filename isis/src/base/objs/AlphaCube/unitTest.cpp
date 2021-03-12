/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include "AlphaCube.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main() {
  Preference::Preferences(true);
  AlphaCube c(4, 8, 2, 3, 1.5, 2.5, 3.5, 5.5);
  cout << "1st Test Alpha" << endl;
  cout << c.AlphaSamples() << endl;
  cout << c.AlphaLines() << endl;
  cout << c.AlphaSample(1.0) << endl;
  cout << c.AlphaLine(1.0) << endl;
  cout << c.AlphaSample(c.BetaSamples()) << endl;
  cout << c.AlphaLine(c.BetaLines()) << endl;
  cout << c.AlphaSample(0.5) << endl;
  cout << c.AlphaLine(0.5) << endl;
  cout << c.AlphaSample(c.BetaSamples() + 0.5) << endl;
  cout << c.AlphaLine(c.BetaLines() + 0.5) << endl;
  cout << endl;

  cout << "1st Test Beta" << endl;
  cout << c.BetaSamples() << endl;
  cout << c.BetaLines() << endl;
  cout << c.BetaSample(1.0) << endl;
  cout << c.BetaLine(1.0) << endl;
  cout << c.BetaSample(c.AlphaSamples()) << endl;
  cout << c.BetaLine(c.AlphaLines()) << endl;
  cout << endl;

  AlphaCube d(2, 3, 2, 4, 1.5, 1.5, 2.5, 3.5);
  cout << "2nd Alpha Test" << endl;
  cout << d.AlphaSamples() << endl;
  cout << d.AlphaLines() << endl;
  cout << d.AlphaSample(1.0) << endl;
  cout << d.AlphaLine(1.0) << endl;
  cout << d.AlphaSample(d.BetaSamples()) << endl;
  cout << d.AlphaLine(d.BetaLines()) << endl;
  cout << d.AlphaSample(0.5) << endl;
  cout << d.AlphaLine(0.5) << endl;
  cout << d.AlphaSample(d.BetaSamples() + 0.5) << endl;
  cout << d.AlphaLine(d.BetaLines() + 0.5) << endl;
  cout << endl;

  cout << "2nd Beta Test" << endl;
  cout << d.BetaSamples() << endl;
  cout << d.BetaLines() << endl;
  cout << d.BetaSample(1.0) << endl;
  cout << d.BetaLine(1.0) << endl;
  cout << d.BetaSample(d.AlphaSamples()) << endl;
  cout << d.BetaLine(d.AlphaLines()) << endl;
  cout << endl;

  c.Rehash(d);
  cout << "3rd Test Alpha" << endl;
  cout << c.AlphaSamples() << endl;
  cout << c.AlphaLines() << endl;
  cout << c.AlphaSample(1.0) << endl;
  cout << c.AlphaLine(1.0) << endl;
  cout << c.AlphaSample(c.BetaSamples()) << endl;
  cout << c.AlphaLine(c.BetaLines()) << endl;
  cout << c.AlphaSample(0.5) << endl;
  cout << c.AlphaLine(0.5) << endl;
  cout << c.AlphaSample(c.BetaSamples() + 0.5) << endl;
  cout << c.AlphaLine(c.BetaLines() + 0.5) << endl;
  cout << endl;

  cout << "3rd Test Beta" << endl;
  cout << c.BetaSamples() << endl;
  cout << c.BetaLines() << endl;
  cout << c.BetaSample(1.0) << endl;
  cout << c.BetaLine(1.0) << endl;
  cout << c.BetaSample(c.AlphaSamples()) << endl;
  cout << c.BetaLine(c.AlphaLines()) << endl;
  cout << endl;

  try {
    Cube cube("$ISISTESTDATA/isis/src/base/unitTestData/isisTruth.cub", "r");
    Pvl &lab = *cube.label();
    lab.clear();
    lab.addObject(PvlObject("IsisCube"));
    PvlObject &isiscube = lab.findObject("IsisCube");
    isiscube.addGroup(PvlGroup("Dimensions"));
    PvlGroup &dims = isiscube.findGroup("Dimensions");
    dims += PvlKeyword("Samples", "4");
    dims += PvlKeyword("Lines", "8");
    c.UpdateGroup(cube);
    cout << lab << endl;
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
