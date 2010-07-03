#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "Cube.h"
#include <string>

using namespace std;
void filter (Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filt);

void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessByQuickFilter Class ... " << endl;
  Isis::ProcessByQuickFilter p;

  // Testing no input cubes
  try {
    p.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // Testing no output cubes
  try {
    p.SetInputCube("FROM");
    p.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }
  
  // Testing mismatched samples
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO",1,1,1);
    p.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // Testing mismatched lines
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO",1,126,1);
    p.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // Testing mismatched bands
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO",126,126,1);
    p.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  // Testing boxcar size too big
  Isis::ProcessByQuickFilter p2;
  Isis::Cube temp;
  temp.SetDimensions(3,3,1);
  temp.Create("/tmp/isisfilterprocess_01");
  temp.Close();
  try {
    Isis::CubeAttributeInput att;
    p2.SetInputCube("/tmp/isisfilterprocess_01",att);
    p2.SetOutputCube("TO");
    p2.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p2.EndProcess();
    cout << endl;
  }
  
  // Testing boxcar size too big
  Isis::ProcessByQuickFilter p3;
  temp.SetDimensions(2,10,1);
  temp.Create("/tmp/isisfilterprocess_01");
  temp.Close();
  try {
    Isis::CubeAttributeInput att;
    p3.SetInputCube("/tmp/isisfilterprocess_01",att);
    p3.SetOutputCube("TO");
    p3.StartProcess (filter);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p3.EndProcess();
    cout << endl;
  }

  // Test something normal
  Isis::ProcessByQuickFilter p4;
  p4.SetInputCube("FROM");
  p4.SetOutputCube("TO");
  p4.StartProcess (filter);
  p4.EndProcess();
  cout << endl;

  // Test something normal
  Isis::ProcessByQuickFilter p5;
  p4.SetInputCube("FROM");
  p4.SetOutputCube("TO");
  p4.SetFilterParameters(9,9);
  p4.StartProcess (filter);
  p4.EndProcess();

  temp.Open("/tmp/isisfilterprocess_01");
  temp.Close(true);
  temp.Open("/tmp/isisfilterprocess_02");
  temp.Close(true);
}

void filter (Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filt) {
  if (in.Line()==1&&in.Band()==1) {
    cout << endl;
    cout << "Boxcar width:   " << filt.Width() << endl;
    cout << "Boxcar height:  " << filt.Height() << endl;
    cout << "Low:            " << filt.Low() << endl;
    cout << "High:           " << filt.High() << endl;
    cout << "Minimum:        " << filt.MinimumPixels() << endl;
    cout << "Samples:        " << filt.Samples() << endl;
    cout << endl;
  }
  cout << "Working on line:  " << in.Line() << endl;
}
