#include <iostream>

#include "Centroid.h"
#include "FileName.h"
#include "IException.h"
#include "iString.h"

#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  try {
    cout << "Unit test for Isis::Centroid" << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
