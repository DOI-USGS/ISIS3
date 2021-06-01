/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include "IException.h"
#include "Preference.h"
#include "SpectralDefinition.h"

using std::cerr;
using std::endl;

using namespace Isis;

class SpectralDefStub : public SpectralDefinition {
  public:
    //empty constructor
    SpectralDefStub() : SpectralDefinition(){
    }

    //default destructor 
    ~SpectralDefStub() {
    }

    //returns a Spectel with s,l,b and then 0's 
    Spectel findSpectel(const int sample, const int line, const int band) const{
      return Spectel(Pixel(sample,line,band,0.1), 0.2, 0.3); 
    }

    //returns the inSpectel
    Spectel findSpectel(const Spectel &inSpectel, int sectionNumber) const{
      return Spectel(inSpectel);
    }

    //returns a Spectel with 0,0,0, 0.0, wavelength, wavelength
    Spectel findSpectelByWavelength(const double wavelength, int sectionNumber) const{
      return Spectel(Pixel(0,0,0,0.0), wavelength, wavelength);
    }

    //returns band #
    int sectionNumber(int s, int l, int b) const{
      return b; 
    }
};


int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(14);

  cerr << "----- Testing SpectralDefinition (Abstract Class)  -----" << endl;

  try {
   SpectralDefStub specDef; 
   cerr << "Sample count: " << specDef.sampleCount() << endl; 
   cerr << "Line count: " << specDef.lineCount() << endl;
   cerr << "Band count: " << specDef.bandCount() << endl; 
   cerr << "Section count: " << specDef.sectionCount() << endl; 

   Spectel spec1 =  specDef.findSpectel(1,2,3);

   cerr << "----- getSpectel(s,l,b) -----" << endl;

   cerr << "Sample: " << spec1.sample() << endl; 
   cerr << "Line: " << spec1.line() << endl; 
   cerr << "Band: " << spec1.band() << endl; 
   cerr << "DN: " << spec1.DN() << endl; 
   cerr << "Wavelength Center: " << spec1.centerWavelength() << endl; 
   cerr << "Filter width: " << spec1.filterWidth() << endl; 

  }
  catch (IException &e) {
    e.print();
  }

}
