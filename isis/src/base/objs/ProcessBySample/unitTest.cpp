/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Isis.h"
#include "ProcessBySample.h"
#include "Cube.h"
#include <string>

using namespace std;

void oneInput(Isis::Buffer &b);
void oneOutput(Isis::Buffer &b);
void oneInAndOut(Isis::Buffer &ob, Isis::Buffer &ib);
void twoInAndOut(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob);


class InPlaceFunctor{
public:
    void operator()(Isis::Buffer &in) const{


        oneInput(in);
    }


};

class InputOutputFunctor{
public:
    void operator()(Isis::Buffer &in, Isis::Buffer &out) const {




        oneInAndOut(in,out);
    }





};


class InputOutputListFunctor{
public:
    void operator()(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob) const {



        twoInAndOut(ib,ob);

    }



};


void IsisMain() {


    Isis::Preference::Preferences(true);
    Isis::ProcessBySample p;

    cout << "Testing StartProcess routines that accept processing functions:" << endl;


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








    cout << "Testing Process routines which accept functors:" << endl;

    InPlaceFunctor inPlace;
    InputOutputFunctor inputOutput;
    InputOutputListFunctor inputOutputList;

    p.SetInputCube("FROM");
    p.ProcessCubeInPlace(inPlace,false);
    p.EndProcess();


    p.SetInputCube("FROM");
    p.SetOutputCube("TO");
    p.ProcessCube(inputOutput,false);
    p.EndProcess();

    p.SetInputCube("FROM");
    p.SetInputCube("FROM2");
    p.SetOutputCube("TO");
    p.SetOutputCube("TO2");
    p.ProcessCubes(inputOutputList,false);
    p.EndProcess();

    Isis::Cube cube;
    cube.open("$temporary/isisProcessBySample_01");
    cube.close(true);
    cube.open("$temporary/isisProcessBySample_02");
    cube.close(true);





}

void oneInput(Isis::Buffer &b) {
  if((b.Sample() == 1) && (b.Band() == 1)) {
    cout << "Testing one input cube ... " << endl;
    cout << "Buffer Samples:  " << b.SampleDimension() << endl;
    cout << "Buffer Lines:    " << b.LineDimension() << endl;
    cout << "Buffer Bands:    " << b.BandDimension() << endl;
    cout << endl;
  }
  cout << "Sample:  " << b.Sample()
       << "  Line:  " << b.Line()
       << "  Band:  " << b.Band() << endl;
}

void oneOutput(Isis::Buffer &b) {
  if((b.Sample() == 1) && (b.Band() == 1)) {
    cout << endl;
    cout << "Testing one output cube ... " << endl;
    cout << "Buffer Samples:  " << b.SampleDimension() << endl;
    cout << "Buffer Lines:    " << b.LineDimension() << endl;
    cout << "Buffer Bands:    " << b.BandDimension() << endl;
    cout << endl;
  }
  cout << "Sample:  " << b.Sample()
       << "  Line:  " << b.Line()
       << "  Band:  " << b.Band() << endl;
}

void oneInAndOut(Isis::Buffer &ib, Isis::Buffer &ob) {
  if((ib.Sample() == 1) && (ib.Band() == 1)) {
    cout << endl;
    cout << "Testing one input and output cube ... " << endl;
    cout << "Buffer Samples:  " << ib.SampleDimension() << endl;
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
