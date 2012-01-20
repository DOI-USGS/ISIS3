#include <iostream>

#include "Centroid.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"

#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  try {
    cout << "Unit test for Isis::Centroid" << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}
