#include <iostream>
#include "iException.h"
#include "System.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::System" << endl;

  try {
    string command = "ls -1 *.cpp *.h";
    Isis::System (command);
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }
}



