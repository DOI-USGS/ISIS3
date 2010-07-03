#include <iostream>
#include <iomanip>
#include "iException.h"
#include "Pixel.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Pixel" << endl;
  
  double d = 0.0;
  cout << "Testing 0.0 ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(d) << endl;
  cout << "IsValid:  " << Isis::Pixel::IsValid(d) << endl;
  cout << "IsNull:   " << Isis::Pixel::IsNull(d) << endl;
  cout << "IsLow:    " << Isis::Pixel::IsLow(d) << endl;
  cout << "IsHigh:   " << Isis::Pixel::IsHigh(d) << endl;
  cout << "IsHrs:    " << Isis::Pixel::IsHrs(d) << endl;
  cout << "IsHis:    " << Isis::Pixel::IsHis(d) << endl;
  cout << "IsLrs:    " << Isis::Pixel::IsLrs(d) << endl;
  cout << "IsLis:    " << Isis::Pixel::IsLis(d) << endl;
  cout << endl;
  
  d = Isis::Null;
  cout << "Testing Isis::Null ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(d) << endl;
  cout << "IsValid:  " << Isis::Pixel::IsValid(d) << endl;
  cout << "IsNull:   " << Isis::Pixel::IsNull(d) << endl;
  cout << "IsLow:    " << Isis::Pixel::IsLow(d) << endl;
  cout << "IsHigh:   " << Isis::Pixel::IsHigh(d) << endl;
  cout << "IsHrs:    " << Isis::Pixel::IsHrs(d) << endl;
  cout << "IsHis:    " << Isis::Pixel::IsHis(d) << endl;
  cout << "IsLrs:    " << Isis::Pixel::IsLrs(d) << endl;
  cout << "IsLis:    " << Isis::Pixel::IsLis(d) << endl;
  cout << endl;

  d = Isis::Lis;
  cout << "Testing Isis::Lis ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(d) << endl;
  cout << "IsValid:  " << Isis::Pixel::IsValid(d) << endl;
  cout << "IsNull:   " << Isis::Pixel::IsNull(d) << endl;
  cout << "IsLow:    " << Isis::Pixel::IsLow(d) << endl;
  cout << "IsHigh:   " << Isis::Pixel::IsHigh(d) << endl;
  cout << "IsHrs:    " << Isis::Pixel::IsHrs(d) << endl;
  cout << "IsHis:    " << Isis::Pixel::IsHis(d) << endl;
  cout << "IsLrs:    " << Isis::Pixel::IsLrs(d) << endl;
  cout << "IsLis:    " << Isis::Pixel::IsLis(d) << endl;
  cout << endl;

  d = Isis::Lrs;
  cout << "Testing Isis::Lrs ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(d) << endl;
  cout << "IsValid:  " << Isis::Pixel::IsValid(d) << endl;
  cout << "IsNull:   " << Isis::Pixel::IsNull(d) << endl;
  cout << "IsLow:    " << Isis::Pixel::IsLow(d) << endl;
  cout << "IsHigh:   " << Isis::Pixel::IsHigh(d) << endl;
  cout << "IsHrs:    " << Isis::Pixel::IsHrs(d) << endl;
  cout << "IsHis:    " << Isis::Pixel::IsHis(d) << endl;
  cout << "IsLrs:    " << Isis::Pixel::IsLrs(d) << endl;
  cout << "IsLis:    " << Isis::Pixel::IsLis(d) << endl;
  cout << endl;

  d = Isis::His;
  cout << "Testing Isis::His ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(d) << endl;
  cout << "IsValid:  " << Isis::Pixel::IsValid(d) << endl;
  cout << "IsNull:   " << Isis::Pixel::IsNull(d) << endl;
  cout << "IsLow:    " << Isis::Pixel::IsLow(d) << endl;
  cout << "IsHigh:   " << Isis::Pixel::IsHigh(d) << endl;
  cout << "IsHrs:    " << Isis::Pixel::IsHrs(d) << endl;
  cout << "IsHis:    " << Isis::Pixel::IsHis(d) << endl;
  cout << "IsLrs:    " << Isis::Pixel::IsLrs(d) << endl;
  cout << "IsLis:    " << Isis::Pixel::IsLis(d) << endl;
  cout << endl;

  d = Isis::Hrs;
  cout << "Testing Hrs ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(d) << endl;
  cout << "IsValid:  " << Isis::Pixel::IsValid(d) << endl;
  cout << "IsNull:   " << Isis::Pixel::IsNull(d) << endl;
  cout << "IsLow:    " << Isis::Pixel::IsLow(d) << endl;
  cout << "IsHigh:   " << Isis::Pixel::IsHigh(d) << endl;
  cout << "IsHrs:    " << Isis::Pixel::IsHrs(d) << endl;
  cout << "IsHis:    " << Isis::Pixel::IsHis(d) << endl;
  cout << "IsLrs:    " << Isis::Pixel::IsLrs(d) << endl;
  cout << "IsLis:    " << Isis::Pixel::IsLis(d) << endl;
  cout << endl;
}

