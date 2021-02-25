/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <iomanip>
#include "IException.h"
#include "Spectel.h"
#include "Preference.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Spectel" << endl;

  cout << "Testing Spectel instantiation with all numerical inputs ..." << endl;

  Isis::Spectel spec(1,2,100,123.45, 0.1, 0.05); 

  cout << "Sample:   " << spec.sample() << endl;
  cout << "Line:   " << spec.line() << endl;
  cout << "Band:   " << spec.band() << endl;
  cout << "DN:   " << spec.DN() << endl;
  cout << "Central wavelength:   " << spec.centerWavelength() << endl;
  cout << "Filter width:   " << spec.filterWidth() << endl;
  cout << endl; 

  cout << "Testing Spectel's copy constructor ..." << endl;

  Isis::Spectel spec2(spec);
  cout << "Sample:   " << spec2.sample() << endl;
  cout << "Line:   " << spec2.line() << endl;
  cout << "Band:   " << spec2.band() << endl;
  cout << "DN:   " << spec2.DN() << endl;
  cout << "Central wavelength:   " << spec2.centerWavelength() << endl;
  cout << "Filter width:   " << spec2.filterWidth() << endl;
  cout << endl; 

  cout << "Testing Spectel's copy assignemnet operator ..." << endl;

  Isis::Spectel spec3;
  spec3 = spec2; 
  cout << "Sample:   " << spec3.sample() << endl;
  cout << "Line:   " << spec3.line() << endl;
  cout << "Band:   " << spec3.band() << endl;
  cout << "DN:   " << spec3.DN() << endl;
  cout << "Central wavelength:   " << spec3.centerWavelength() << endl;
  cout << "Filter width:   " << spec3.filterWidth() << endl;
  cout << endl; 

  cout << "Testing Spectel's constructor that takes a Pixel ..." << endl;

  Isis::Spectel spec4(Isis::Pixel(1,2,3,0.4), 0.5, 0.6);
  cout << "Sample:   " << spec4.sample() << endl;
  cout << "Line:   " << spec4.line() << endl;
  cout << "Band:   " << spec4.band() << endl;
  cout << "DN:   " << spec4.DN() << endl;
  cout << "Central wavelength:   " << spec4.centerWavelength() << endl;
  cout << "Filter width:   " << spec4.filterWidth() << endl;
  cout << endl; 

}

