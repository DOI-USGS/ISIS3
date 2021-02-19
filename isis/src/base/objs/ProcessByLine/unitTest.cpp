/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"
#include "ProcessByLine.h"
#include "Cube.h"
#include <iostream>
#include <string>

using namespace std;
void oneInput(Isis::Buffer &b);
void oneOutput(Isis::Buffer &b);
void oneInAndOut(Isis::Buffer &ob, Isis::Buffer &ib);
void twoInAndOut(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob);

class Functor1 {
  public:
    void operator()(Isis::Buffer & in) const {
      oneInput(in);
    };
};

class Functor2 {
  public:
  void operator()(Isis::Buffer & in, Isis::Buffer & out) const {
    oneInAndOut(in, out);
  };
};

class Functor3 {
  public:
  void operator()(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob) const {
    twoInAndOut(ib, ob);
  };
};
void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessByLine Class ... " << endl;
  Isis::ProcessByLine p;

  p.SetInputCube("FROM");
  p.StartProcess(oneInput);
  p.EndProcess();

  p.SetOutputCube("TO", 10, 20, 3);
  p.StartProcess(oneOutput);
  p.EndProcess();

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  p.StartProcess(oneInAndOut);
  p.EndProcess();

  p.SetInputCube("FROM");
  p.SetInputCube("FROM2");
  p.SetOutputCube("TO");
  p.SetOutputCube("TO2");
  p.StartProcess(twoInAndOut);
  p.EndProcess();
  
  cout << "Testing for Functors\n";
  Functor1 func1;
  Functor2 func2;
  Functor3 func3;
  p.SetInputCube("FROM");
  cout << "Functor1\n";
  p.ProcessCubeInPlace(func1, false);
  p.EndProcess();

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  cout << "Functor2\n";
  p.ProcessCube(func2, false);
  p.EndProcess();

  p.SetInputCube("FROM");
  p.SetInputCube("FROM2");
  p.SetOutputCube("TO");
  p.SetOutputCube("TO2");
  cout << "Functor3\n";
  p.ProcessCubes(func3, false);
  p.EndProcess();
  
  cout << "End Testing Functors\n";

  try {
    cout << "Testing error for no input/output ..." << endl;
    p.StartProcess(oneInput);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO");
    cout << "Testing error for too many input/outputs ..." << endl;
    p.StartProcess(oneInput);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    cout << "Testing for exactly one input ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
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
    p.SetOutputCube("TO", 1, 1, 1);
    cout << "Testing for lines mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

#if 0
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 1, 126, 1);
    cout << "Testing for samples mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }
#endif

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO", 126, 126, 1);
    cout << "Testing for bands mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    cout << "Testing for no inputs/outputs ..." << endl;
    p.StartProcess(twoInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

#if 0
  try {
    cout << "Testing for output samples mismatch ..." << endl;
    p.SetOutputCube("TO", 2, 2, 2);
    p.SetOutputCube("TO2", 1, 1, 1);
    p.StartProcess(twoInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }
#endif

  try {
    cout << "Testing for output lines mismatch ..." << endl;
    p.SetOutputCube("TO", 2, 2, 2);
    p.SetOutputCube("TO2", 2, 1, 1);
    p.StartProcess(twoInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }

  try {
    cout << "Testing for output bands mismatch ..." << endl;
    p.SetOutputCube("TO", 2, 2, 2);
    p.SetOutputCube("TO2", 2, 2, 1);
    p.StartProcess(twoInAndOut);
  }
  catch(Isis::IException &e) {
    e.print();
    p.EndProcess();
    cout << endl;
  }


  Isis::Cube cube;
  cube.open("$temporary/isisProcessByLine_01");
  cube.close(true);
  cube.open("$temporary/isisProcessByLine_02");
  cube.close(true);
}

void oneInput(Isis::Buffer &b) {
  if((b.Line() == 1) && (b.Band() == 1)) {
    cout << "Testing one input cube ... " << endl;
    cout << "Buffer Samples:  " << b.size() << endl;
    cout << "Buffer Lines:    " << b.LineDimension() << endl;
    cout << "Buffer Bands:    " << b.BandDimension() << endl;
    cout << endl;
  }
  cout << "Sample:  " << b.Sample()
       << "  Line:  " << b.Line()
       << "  Band:  " << b.Band() << endl;
}

void oneOutput(Isis::Buffer &b) {
  if((b.Line() == 1) && (b.Band() == 1)) {
    cout << endl;
    cout << "Testing one output cube ... " << endl;
    cout << "Buffer Samples:  " << b.size() << endl;
    cout << "Buffer Lines:    " << b.LineDimension() << endl;
    cout << "Buffer Bands:    " << b.BandDimension() << endl;
    cout << endl;
  }
  cout << "Sample:  " << b.Sample()
       << "  Line:  " << b.Line()
       << "  Band:  " << b.Band() << endl;
}

void oneInAndOut(Isis::Buffer &ib, Isis::Buffer &ob) {
  if((ib.Line() == 1) && (ib.Band() == 1)) {
    cout << endl;
    cout << "Testing one input and output cube ... " << endl;
    cout << "Buffer Samples:  " << ib.size() << endl;
    cout << "Buffer Lines:    " << ib.LineDimension() << endl;
    cout << "Buffer Bands:    " << ib.BandDimension() << endl;
    cout << endl;
  }
  cout << "Sample:  " << ib.Sample()
       << "  Line:  " << ib.Line()
       << "  Band:  " << ib.Band() << endl;

  if((ib.Sample() != ob.Sample()) ||
      (ib.Line() != ob.Line()) ||
      (ib.Band() != ob.Band())) {
    cout << "Bogus error #1" << endl;
  }
}

void twoInAndOut(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob) {
  static bool firstTime = true;
  if(firstTime) {
    firstTime = false;
    cout << "Testing two input and output cubes ... " << endl;
    cout << "Number of input cubes:   " << ib.size() << endl;
    cout << "Number of output cubes:  " << ob.size() << endl;
    cout << endl;
  }

  Isis::Buffer *inone = ib[0];
  Isis::Buffer *intwo = ib[1];
  Isis::Buffer *outone = ob[0];
  Isis::Buffer *outtwo = ob[1];

  cout << "Sample:  " << inone->Sample() << ":" << intwo->Sample()
       << "  Line:  " << inone->Line() << ":" << intwo->Line()
       << "  Band:  " << inone->Band() << ":" << intwo->Band() << endl;

  if((inone->Sample() != intwo->Sample()) ||
      (inone->Line() != intwo->Line())) {
    cout << "Bogus error #1" << endl;
  }

  if((inone->Sample() != outone->Sample()) ||
      (inone->Line() != outone->Line()) ||
      (inone->Band() != outone->Band())) {
    cout << "Bogus error #2" << endl;
  }

  if((outone->Sample() != outtwo->Sample()) ||
      (outone->Line() != outtwo->Line()) ||
      (outone->Band() != outtwo->Band())) {
    cout << "Bogus error #3" << endl;
  }
}
