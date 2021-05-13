/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include "IException.h"
#include "Preference.h"
#include "SpectralDefinitionFactory.h"
#include "FileName.h"

using std::cerr;
using std::endl;

using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(14);

  cerr << "----- Testing SpectralDefinitionFactory -----" << endl;

  try {
   SpectralDefinitionFactory::NewSpectralDefinition(FileName("assets/test.csv"));
  }
  catch (IException &e) {
    e.print();
  }

  try {
    SpectralDefinitionFactory::NewSpectralDefinition(FileName("assets/test.txt"));
  } 
  catch (IException &e) {
    e.print();
  }

  try {
    SpectralDefinitionFactory::NewSpectralDefinition(FileName("assets/cube.cub"));
  } 
  catch (IException &e) {
    e.print();
  }
}
