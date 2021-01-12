#include <string>
#include <iostream>
#include <stdio.h>

#include "BandManager.h"
#include "Cube.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  QString fname = "$base/testData/isisTruth.cub";
  Isis::Cube cube;
  cube.open(fname);

  Isis::BandManager band(cube);
  cout << "Buffer Size:  " <<
       band.SampleDimension() << " " <<
       band.LineDimension() << " " <<
       band.BandDimension() << endl;
  cout << endl;

  for(band.begin(); !band.end(); band++) {
    cout << "  Current sample, line, band is:  "
         << band.Sample() << " "
         << band.Line() << " "
         << band.Band() << endl;
  }
  cout << endl;

  Isis::BandManager bandReverse(cube, true);
  cout << "Buffer Size:  " <<
       bandReverse.SampleDimension() << " " <<
       bandReverse.LineDimension() << " " <<
       bandReverse.BandDimension() << endl;
  cout << endl;

  for(bandReverse.begin(); !bandReverse.end(); bandReverse++) {
    cout << "  Current sample, line, band is:  "
         << bandReverse.Sample() << " "
         << bandReverse.Line() << " "
         << bandReverse.Band() << endl;
  }
  cout << endl;

  band.SetBand(50);
  cout << "  Current sample, line, band is:  "
       << band.Sample() << " "
       << band.Line() << " "
       << band.Band() << endl;
  cout << endl;

  bandReverse.SetBand(50);
  cout << "  Current sample, line, band is:  "
       << bandReverse.Sample() << " "
       << bandReverse.Line() << " "
       << bandReverse.Band() << endl;
  cout << endl;

  try {
    cout << "Testing errors ... " << endl;
    band.SetBand(0, 0);
  }
  catch(Isis::IException &e) {
    e.print();
    cout << endl;
  }

  try {
    cout << "Testing errors ... " << endl;
    band.SetBand(1, 0);
  }
  catch(Isis::IException &e) {
    e.print();
    cout << endl;
  }

  cube.close();
}
