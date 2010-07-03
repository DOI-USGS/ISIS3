#include <string>
#include <iostream>
#include <stdio.h>

#include "Preference.h"
#include "iException.h"
#include "Cube.h"
#include "LineManager.h"

using namespace std;
int main (int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  string fname = "$base/testData/isisTruth.cub";
  Isis::Cube cube;
  cube.Open (fname);
  
  Isis::LineManager line(cube);
  cout << "Buffer Size:  " << 
          line.SampleDimension() << " " <<
          line.LineDimension() << " " <<
          line.BandDimension() << endl;
  cout << endl;

  for (line.begin(); !line.end(); line++) {
    cout << "  Current sample, line, band is:  "
         << line.Sample() << " "
         << line.Line() << " "
         << line.Band() << endl;
  }
  cout << endl;

  Isis::LineManager lineReverse(cube,true);
  cout << "Buffer Size:  " << 
          lineReverse.SampleDimension() << " " <<
          lineReverse.LineDimension() << " " <<
          lineReverse.BandDimension() << endl;
  cout << endl;

  for (lineReverse.begin(); !lineReverse.end(); lineReverse++) {
    cout << "  Current sample, line, band is:  "
         << lineReverse.Sample() << " "
         << lineReverse.Line() << " "
         << lineReverse.Band() << endl;
  }
  cout << endl;

  line.SetLine(50);
  cout << "  Current sample, line, band is:  "
       << line.Sample() << " "
       << line.Line() << " "
       << line.Band() << endl;
  cout << endl;

  try {
    cout << "Testing errors ... " << endl;
    line.SetLine(0,0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    cout << endl;
  }

  try {
    cout << "Testing errors ... " << endl;
    line.SetLine(1,0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    cout << endl;
  }

  cube.Close ();
}
