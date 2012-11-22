#include <iostream>
#include "Preference.h"

using namespace std;

int main() {
  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessExportPds ..." << endl;
  cout << "Test deferred to the appTest for isis2pds" << endl;
  cout << "ExportTable() with detached tables is tested in hideal2pds." << endl;

  return (0);
}
