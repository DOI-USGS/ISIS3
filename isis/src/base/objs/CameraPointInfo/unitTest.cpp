#include "CameraPointInfo.h"
#include "Preference.h"
#include "PvlGroup.h"

#include <iostream>
#include <iomanip>

using namespace std;

int main() {
  Isis::Preference::Preferences(true);

  // The class being tested
  Isis::CameraPointInfo cpi;
 
  // It is necessary to delete the Filename keyword for the test to pass
  // this is because the directory it is run from may change
  // under normal usage Filename is always included
   
  cpi.SetCube("unitTest1.cub");
  Isis::PvlGroup *grp = cpi.SetImage(1, 1);
  grp->DeleteKeyword("Filename");
  cout << (*grp) << endl << endl;

  cpi.SetCube("unitTest1.cub");
  Isis::PvlGroup *too = cpi.SetGround(-84.5, 15.0);
  too->DeleteKeyword("Filename");
  cout << (*too);

  // We have ownership, so, delete
  delete grp;
  grp = NULL;
  delete too;
  too = NULL;
  return 0;
}
