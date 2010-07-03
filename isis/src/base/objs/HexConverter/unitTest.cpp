#include "HexConverter.h"

#include <iostream>

#include "iString.h"

using namespace std;
using namespace Isis;

int main() {
  iString testStr = "Hello, This is A C#$$@Oo))*$@l \tString";
  cout << "Original String:    " << testStr << "\n";
  testStr = HexConverter::ToHex(testStr);
  cout << "Hex String:         " << testStr << "\n";
  testStr = HexConverter::ToString(testStr);
  cout << "Reconverted String: " << testStr << "\n";
}
