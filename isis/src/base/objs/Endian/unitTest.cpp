#include <iostream>
#include "Endian.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for IsisEndian" << endl;

  if (Isis::IsLittleEndian() == true) {
    cout << Isis::IsLittleEndian() << endl;
    cout << Isis::IsBigEndian() << endl;
  }
  else {
    cout << Isis::IsBigEndian() << endl;
    cout << Isis::IsLittleEndian() << endl;
  }

  if (Isis::IsLsb() == true) {
    cout << Isis::IsLsb() << endl;
    cout << Isis::IsMsb() << endl;
  }
  else {
    cout << Isis::IsMsb() << endl;
    cout << Isis::IsLsb() << endl;
  }

  cout << Isis::ByteOrderName(Isis::ByteOrderEnumeration("msb")) << endl;
  cout << Isis::ByteOrderName(Isis::ByteOrderEnumeration("lsb")) << endl;
}
