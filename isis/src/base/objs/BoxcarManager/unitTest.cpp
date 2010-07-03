#include <string>
#include <iostream>
#include <stdio.h>

#include "Preference.h"
#include "Cube.h"
#include "BoxcarManager.h"

using namespace std;

int main (int argc, char *argv[]) {

  Isis::Preference::Preferences(true);
    
  string fname = "$base/testData/isisTruth.cub";
  Isis::Cube cube;
  cube.Open (fname);
  
  //  Test 5x5 boxcar
	Isis::BoxcarManager box5x5(cube,5,5);
  cout << "Buffer (Boxcar) Size:  " << 
          box5x5.SampleDimension() << " " <<
          box5x5.LineDimension() << " " <<
          box5x5.BandDimension() << endl;
  cout << endl;

  for (box5x5.begin(); !box5x5.end(); box5x5++) {
    if (box5x5.Sample() <= 0) {
			cout << "  Coordinates of upper left corner of boxcar, sample, line, band is:  "
           << box5x5.Sample() << " "
           << box5x5.Line() << " "
           << box5x5.Band() << endl;
		}
  }
	cout << endl;

	//  Test 4x4 boxcar
 	Isis::BoxcarManager box4x4(cube,4,4);
  cout << "Buffer (Boxcar) Size:  " << 
          box4x4.SampleDimension() << " " <<
          box4x4.LineDimension() << " " <<
          box4x4.BandDimension() << endl;
  cout << endl;

  for (box4x4.begin(); !box4x4.end(); box4x4++) {
    if (box4x4.Sample() <= 0) {
			cout << " Coordinates of upper left corner of boxcar,  sample, line, band is:  "
           << box4x4.Sample() << " "
           << box4x4.Line() << " "
           << box4x4.Band() << endl;
		}
	}
	cout << endl;

  cube.Close ();
}
