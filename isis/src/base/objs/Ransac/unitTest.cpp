#include <iostream>
#include <string>
#include <QString>

#include "Ransac.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"

#include "Preference.h"

using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);
  
  try {
    cout << "Unit test for Isis::Ransac" << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
