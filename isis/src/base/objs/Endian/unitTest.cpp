#include <iostream>
#include "Endian.h"
#include "IString.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "Unit test for IsisEndian" << endl;

  if(IsLittleEndian() == true) {
    cout << IsLittleEndian() << endl;
    cout << IsBigEndian() << endl;
  }
  else {
    cout << IsBigEndian() << endl;
    cout << IsLittleEndian() << endl;
  }

  if(IsLsb() == true) {
    cout << IsLsb() << endl;
    cout << IsMsb() << endl;
  }
  else {
    cout << IsMsb() << endl;
    cout << IsLsb() << endl;
  }

  cout << ByteOrderName(ByteOrderEnumeration("msb")) << endl;
  cout << ByteOrderName(ByteOrderEnumeration("lsb")) << endl;
}
