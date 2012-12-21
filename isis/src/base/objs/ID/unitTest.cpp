#include <iostream>

#include <QString>

#include "IException.h"
#include "IString.h"
#include "ID.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main() {
  Preference::Preferences(true);
  cout << "Test One: core test and limit test" << endl;
  try {
    ID pid("ABCD??EFG");
    for(int i = 0; i < 100; i++) {
      QString test = pid.Next();
      if(i % 10 == 0) {
        cout << test << endl;
      }
    }
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Test 2: No '?' test" << endl;
  try {
    ID pid2("Serial");
    for(int i = 0; i < 5; i++) {
      cout << pid2.Next() << endl;
    }
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Test 3: Broken replacement string" << endl;
  try {
    ID pid3("Serial??Number??");
    for(int i = 0; i < 5; i++) {
      cout << pid3.Next() << endl;
    }
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Test 4: differing start numbers" << endl;
  try {
    ID pid4("Test??", 0);
    for(int i = 0; i < 5; i++) {
      cout << pid4.Next() << endl;
    }
  }
  catch(IException &e) {
    e.print();
  }
  return 0;
}
