#include <iostream>

#include "CentroidApolloPan.h"
#include "FileName.h"
#include "IException.h"
#include "iString.h"

#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  try {
    cout << "Unit test for Isis::CentroidApolloPan" << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
