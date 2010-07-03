#include <iostream>

#include "HiLab.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(){
  Isis::Preference::Preferences(true);

  //create a dummy Isis::Progress object
  Cube cube;
  cube.Open("red3Test.cub");

  cout << "Testing constructor ...\n";
  HiLab hiLab(&cube);
  cout << "Testing getCpmmNumber() ...\n";
  cout << "CpmmNumber " << hiLab.getCpmmNumber() <<endl;
  cout << "Testing getChannel() ...\n";
  cout << "Channel "  << hiLab.getChannel() <<endl;
  cout << "Testing getBin() ...\n";
  cout << "Bin " << hiLab.getBin() << endl;
  cout << "Testing getTdi() ...\n";
  cout << "Tdi " << hiLab.getTdi() << endl;
  cout << "Testing getCcd() ...\n";
  cout << "Ccd " << hiLab.getCcd() << endl;

  return 0;
}
