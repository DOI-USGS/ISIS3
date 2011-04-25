#include "Isis.h"
#include "ProcessByBrick.h"
#include "Cube.h"
#include <iostream>
#include <string>

using namespace std;
void oneInAndOut(Isis::Buffer &ob, Isis::Buffer &ib);
void twoInAndOut(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob);

class Functor2 {
  public:
  void operator()(Isis::Buffer & in, Isis::Buffer & out){
    oneInAndOut(in, out);
  };
};

class Functor3 {
  public:
  void operator()(vector<Isis::Buffer *> &ib, vector<Isis::Buffer *> &ob){
    twoInAndOut(ib, ob);
  };
};

void IsisMain() {
  Isis::Preference::Preferences(true);
  Isis::ProcessByBrick p;

  Isis::Cube *icube =p.SetInputCube("FROM");
  p.SetBrickSize(10, 10, 2);
  p.SetOutputCube("TO", icube->Samples(), icube->Lines(), icube->Bands());
  Functor2 func2;
  cout << "\nTesting Functors\nFunctor2\n";
  p.StartProcessIO(func2);
  p.EndProcess();
  
  icube = p.SetInputCube("FROM");
  p.SetInputCube("FROM2");
  p.SetBrickSize(10, 10, 2);
  p.SetOutputCube("TO", icube->Samples(), icube->Lines(), icube->Bands());
  p.SetOutputCube("TO2", icube->Samples(), icube->Lines(), icube->Bands());

  Functor3 func3;
  cout << "\nFunctor3\n";
  p.StartProcessIOList(func3);
  cout << "End Testing Functors\n\n";
  
  p.StartProcess(twoInAndOut);
  p.EndProcess();
  
  Isis::Cube cube;
  cube.Open("/tmp/isisProcessByBrick_01");
  cube.Close(true);
  cube.Open("/tmp/isisProcessByBrick_02");
  cube.Close(true);
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

  Isis::Buffer &inone = *ib[0];
  Isis::Buffer &intwo = *ib[1];
  Isis::Buffer &outone = *ob[0];
  Isis::Buffer &outtwo = *ob[1];

  cout << "Sample:  " << inone.Sample() << ":" << intwo.Sample()
       << "  Line:  " << inone.Line() << ":" << intwo.Line()
       << "  Band:  " << inone.Band() << ":" << intwo.Band() << endl;

  if((inone.Sample() != intwo.Sample()) ||
      (inone.Line() != intwo.Line())) {
    cout << "Bogus error #1" << endl;
  }

  if((inone.Sample() != outone.Sample()) ||
      (inone.Line() != outone.Line()) ||
      (inone.Band() != outone.Band())) {
    cout << "Bogus error #2" << endl;
  }

  if((outone.Sample() != outtwo.Sample()) ||
      (outone.Line() != outtwo.Line()) ||
      (outone.Band() != outtwo.Band())) {
    cout << "Bogus error #3" << endl;
  }
}
