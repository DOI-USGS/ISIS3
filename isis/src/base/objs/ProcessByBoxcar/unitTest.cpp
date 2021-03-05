/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"
#include "ProcessByBoxcar.h"
#include <string>

using namespace std;
void oneInAndOut(Isis::Buffer &ib, double &ob);

void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessByBoxcar Class ... " << endl;
  Isis::ProcessByBoxcar p;

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  p.SetBoxcarSize(3, 3);
  p.StartProcess(oneInAndOut);
  p.EndProcess();

  try {
    cout << "Testing for no inputs/outputs ..." << endl;
    p.SetBoxcarSize(3, 3);
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetBoxcarSize(3, 3);
    cout << "Testing for exactly one input ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetOutputCube("TO");
    p.SetBoxcarSize(3, 3);
    cout << "Testing for exactly one output ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetInputCube("FROM2");
    p.SetOutputCube("TO");
    p.SetBoxcarSize(3, 3);
    cout << "Testing for too many input cubes ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO");
    p.SetOutputCube("TO2");
    p.SetBoxcarSize(3, 3);
    cout << "Testing for too many output cubes ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 1, 1, 1);
    p.SetBoxcarSize(3, 3);
    cout << "Testing for lines mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 1, 126, 1);
    p.SetBoxcarSize(3, 3);
    cout << "Testing for samples mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 126, 126, 1);
    p.SetBoxcarSize(3, 3);
    cout << "Testing for bands mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO");
    cout << "Testing for boxcar size not set ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  Isis::Cube cube;
  cube.open("$temporary/isisProcessByBoxcar_01");
  cube.close(true);
  cube.open("$temporary/isisProcessByBoxcar_02");
  cube.close(true);

}

void oneInAndOut(Isis::Buffer &ib, double &ob) {
  static bool firstTime = true;
  if(firstTime) {
    firstTime = false;
    cout << endl;
    cout << "Testing one input and output cube ... " << endl;
    cout << "Boxcar Samples:  " << ib.SampleDimension() << endl;
    cout << "Boxcar Lines:    " << ib.LineDimension() << endl;
    cout << "Boxcar Bands:    " << ib.BandDimension() << endl;
    cout << endl;
  }

  if(ib.Sample() < 1) {
    cout << "Top Left Sample:  " << ib.Sample()
         << ", Top Left Line:  " << ib.Line()
         << ", Top Left Band:  " << ib.Band() << endl;
  }
}

