#include <iostream>
#include "PixelType.h"
#include "Preference.h"

using namespace Isis;
using namespace std;
int main() {
  Preference::Preferences(true);

  cout << SizeOf(None) << endl;
  cout << SizeOf(UnsignedByte) << endl;
  cout << SizeOf(SignedByte) << endl;
  cout << SizeOf(UnsignedWord) << endl;
  cout << SizeOf(SignedWord) << endl;
  cout << SizeOf(UnsignedInteger) << endl;
  cout << SizeOf(SignedInteger) << endl;
  cout << SizeOf(Real) << endl;
  cout << SizeOf(Double) << endl;
  cout << endl;
  cout << PixelTypeName(None) << endl;
  cout << PixelTypeName(UnsignedByte) << endl;
  cout << PixelTypeName(SignedByte) << endl;
  cout << PixelTypeName(UnsignedWord) << endl;
  cout << PixelTypeName(SignedWord) << endl;
  cout << PixelTypeName(UnsignedInteger) << endl;
  cout << PixelTypeName(SignedInteger) << endl;
  cout << PixelTypeName(Real) << endl;
  cout << PixelTypeName(Double) << endl;
  cout << endl;
  cout << PixelTypeEnumeration("None") << endl;
  cout << PixelTypeEnumeration("UnsignedByte") << endl;
  cout << PixelTypeEnumeration("SignedByte") << endl;
  cout << PixelTypeEnumeration("UnsignedWord") << endl;
  cout << PixelTypeEnumeration("SignedWord") << endl;
  cout << PixelTypeEnumeration("UnsignedInteger") << endl;
  cout << PixelTypeEnumeration("SignedInteger") << endl;
  cout << PixelTypeEnumeration("Real") << endl;
  cout << PixelTypeEnumeration("Double") << endl;
}

