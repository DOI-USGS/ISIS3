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
