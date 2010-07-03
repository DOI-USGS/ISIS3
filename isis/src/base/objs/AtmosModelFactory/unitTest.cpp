#include <iostream>
#include <cstdlib>
#include "Preference.h"

using namespace std;

int main () {
  Isis::Preference::Preferences(true);
  cout << "All testing deferred to AtmosModel and it's extended classes." << endl;

  return 0;
}
