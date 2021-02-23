/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "Cube.h"
#include <string>

using namespace std;
void filter(Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filt);

void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessByQuickFilter Class ... " << endl;
  Isis::ProcessByQuickFilter p;

  // Testing no input cubes
  try {
    p.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // Testing no output cubes
  try {
    p.SetInputCube("FROM");
    p.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // Testing mismatched samples
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 1, 1, 1);
    p.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // Testing mismatched lines
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 1, 126, 1);
    p.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // Testing mismatched bands
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 126, 126, 1);
    p.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  // Testing boxcar size too big
  Isis::ProcessByQuickFilter p2;
  Isis::Cube temp;
  temp.setDimensions(3, 3, 1);
  temp.create("$temporary/isisfilterprocess_01");
  temp.close();
  try {
    Isis::CubeAttributeInput att;
    p2.SetInputCube("$temporary/isisfilterprocess_01", att);
    p2.SetOutputCube("TO");
    p2.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p2.EndProcess();
    cout << endl;
  }

  // Testing boxcar size too big
  Isis::ProcessByQuickFilter p3;
  temp.setDimensions(2, 10, 1);
  temp.create("$temporary/isisfilterprocess_01");
  temp.close();
  try {
    Isis::CubeAttributeInput att;
    p3.SetInputCube("$temporary/isisfilterprocess_01", att);
    p3.SetOutputCube("TO");
    p3.StartProcess(filter);
  }
  catch(Isis::IException &e) {
    e.print();
    p3.EndProcess();
    cout << endl;
  }

  // Test something normal
  Isis::ProcessByQuickFilter p4;
  p4.SetInputCube("FROM");
  p4.SetOutputCube("TO");
  p4.StartProcess(filter);
  p4.EndProcess();
  cout << endl;

  // Test something normal
  Isis::ProcessByQuickFilter p5;
  p4.SetInputCube("FROM");
  p4.SetOutputCube("TO");
  p4.SetFilterParameters(9, 9);
  p4.StartProcess(filter);
  p4.EndProcess();

  temp.open("$temporary/isisfilterprocess_01");
  temp.close(true);
  temp.open("$temporary/isisfilterprocess_02");
  temp.close(true);
}

void filter(Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filt) {
  if(in.Line() == 1 && in.Band() == 1) {
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
