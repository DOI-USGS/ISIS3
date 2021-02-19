/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>
#include "IException.h"
#include "Pixel.h"
#include "Preference.h"

using namespace std;
int main(int argc, char *argv[]) {
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

  float f = 0.0; 
  cout << "Testing float 0.0 ... " << endl;
  cout << "IsSpecial:     " << Isis::Pixel::IsSpecial(f) << endl;


  cout << "Testing Pixel instantiation with normal DN ..." << endl;

  Isis::Pixel pix(1,2,100, 123.45); 

  cout << "Sample:   " << pix.sample() << endl;
  cout << "Line:   " << pix.line() << endl;
  cout << "Band:   " << pix.band() << endl;
  cout << "DN:   " << pix.DN() << endl;

  cout << "IsSpecial: " << pix.IsSpecial() << endl;
  cout << "IsValid: " << pix.IsValid() << endl; 
  cout << "IsNull: " << pix.IsNull() << endl; 
  cout << "IsHigh: " << pix.IsHigh() << endl; 
  cout << "IsLow: " << pix.IsLow() << endl; 
  cout << "IsHrs: " << pix.IsHrs() << endl; 
  cout << "IsHis: " << pix.IsHis() << endl; 
  cout << "IsLrs: " << pix.IsLrs() << endl; 
  cout << "IsLis: " << pix.IsLis() << endl; 
  cout << "ToString: " << pix.ToString() << endl; 
  cout << "ToFloat: " << pix.ToFloat() << endl; 
  cout << "ToDouble: " << pix.ToDouble() << endl; 
  cout << "To8Bit: " << static_cast<unsigned>(pix.To8Bit()) << endl; 
  cout << "To16Bit: " << pix.To16Bit() << endl; 
  cout << "To32Bit: " << pix.To32Bit() << endl; 
  cout << endl; 

  cout << "Testing Pixel instantiation with Hrs ..." << endl;
  
  Isis::Pixel pix2(1,20,37, Isis::Hrs);   
  
  cout << "Sample:   " << pix2.sample() << endl;
  cout << "Line:   " << pix2.line() << endl;
  cout << "Band:   " << pix2.band() << endl;
  cout << "DN:   " << pix2.DN() << endl;

  cout << "IsSpecial: " << pix2.IsSpecial() << endl;
  cout << "IsValid: " << pix2.IsValid() << endl; 
  cout << "IsNull: " << pix2.IsNull() << endl; 
  cout << "IsHigh: " << pix2.IsHigh() << endl; 
  cout << "IsLow: " << pix2.IsLow() << endl; 
  cout << "IsHrs: " << pix2.IsHrs() << endl; 
  cout << "IsHis: " << pix2.IsHis() << endl; 
  cout << "IsLrs: " << pix2.IsLrs() << endl; 
  cout << "IsLis: " << pix2.IsLis() << endl; 
  cout << "ToString: " << pix2.ToString() << endl; 
  cout << "ToDouble: " << pix2.ToDouble() << endl; 
  cout << "ToFloat: " << (pix2.ToFloat() == Isis::HIGH_REPR_SAT4) << endl; 
  cout << "To8Bit: " << (pix2.To8Bit() == Isis::HIGH_REPR_SAT1) << endl; 
  cout << "To16Bit: " << (pix2.To16Bit() == Isis::HIGH_REPR_SAT2) << endl; 
  cout << "To32Bit: " << (pix2.To32Bit() ==  Isis::HIGH_REPR_SAT4) << endl; 
  cout << endl; 

  cout << "Testing empty constructor" << endl; 

  Isis::Pixel pix3; 
  cout << "Sample:   " << pix3.sample() << endl;
  cout << "Line:   " << pix3.line() << endl;
  cout << "Band:   " << pix3.band() << endl;
  cout << "DN:   " << pix3.DN() << endl;
  cout << endl; 

  cout << "Testing copy constructor" << endl; 

  Isis::Pixel pix4(pix2);
  cout << "Sample:   " << pix4.sample() << endl;
  cout << "Line:   " << pix4.line() << endl;
  cout << "Band:   " << pix4.band() << endl;
  cout << "DN:   " << pix4.DN() << endl;
  cout << endl; 

  cout << "Testing copy assignment operator" << endl; 

  Isis::Pixel pix5;
  pix5 = pix3;
  cout << "Sample:   " << pix5.sample() << endl;
  cout << "Line:   " << pix5.line() << endl;
  cout << "Band:   " << pix5.band() << endl;
  cout << "DN:   " << pix5.DN() << endl;
  cout << endl; 


}

