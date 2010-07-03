#include <iostream>
#include "PixelType.h"
#include "Preference.h"

using namespace std;
int main () {
  Isis::Preference::Preferences(true);

  cout << Isis::SizeOf(Isis::None) << endl;
  cout << Isis::SizeOf(Isis::UnsignedByte) << endl;
  cout << Isis::SizeOf(Isis::SignedByte) << endl;
  cout << Isis::SizeOf(Isis::UnsignedWord) << endl;
  cout << Isis::SizeOf(Isis::SignedWord) << endl;
  cout << Isis::SizeOf(Isis::UnsignedInteger) << endl;
  cout << Isis::SizeOf(Isis::SignedInteger) << endl;
  cout << Isis::SizeOf(Isis::Real) << endl;
  cout << Isis::SizeOf(Isis::Double) << endl;
  cout << endl;
  cout << Isis::PixelTypeName(Isis::None) << endl;
  cout << Isis::PixelTypeName(Isis::UnsignedByte) << endl;
  cout << Isis::PixelTypeName(Isis::SignedByte) << endl;
  cout << Isis::PixelTypeName(Isis::UnsignedWord) << endl;
  cout << Isis::PixelTypeName(Isis::SignedWord) << endl;
  cout << Isis::PixelTypeName(Isis::UnsignedInteger) << endl;
  cout << Isis::PixelTypeName(Isis::SignedInteger) << endl;
  cout << Isis::PixelTypeName(Isis::Real) << endl;
  cout << Isis::PixelTypeName(Isis::Double) << endl;
  cout << endl;
  cout << Isis::PixelTypeEnumeration("None") << endl;
  cout << Isis::PixelTypeEnumeration("UnsignedByte") << endl;
  cout << Isis::PixelTypeEnumeration("SignedByte") << endl;
  cout << Isis::PixelTypeEnumeration("UnsignedWord") << endl;
  cout << Isis::PixelTypeEnumeration("SignedWord") << endl;
  cout << Isis::PixelTypeEnumeration("UnsignedInteger") << endl;
  cout << Isis::PixelTypeEnumeration("SignedInteger") << endl;
  cout << Isis::PixelTypeEnumeration("Real") << endl;
  cout << Isis::PixelTypeEnumeration("Double") << endl;
}

