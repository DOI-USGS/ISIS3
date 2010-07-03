#include "Isis.h"
#include "ProcessByBoxcar.h"
#include <string>

using namespace std;
void oneInAndOut (Isis::Buffer &ib, double &ob);

void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessByBoxcar Class ... " << endl;
  Isis::ProcessByBoxcar p;

  p.SetInputCube("FROM");
  p.SetOutputCube("TO");
  p.SetBoxcarSize (3,3);
  p.StartProcess(oneInAndOut);
  p.EndProcess();
  
  try {
    cout << "Testing for no inputs/outputs ..." << endl;
    p.SetBoxcarSize (3,3);
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube ("FROM");
    p.SetBoxcarSize (3,3);
    cout << "Testing for exactly one input ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetOutputCube("TO");
    p.SetBoxcarSize (3,3);
    cout << "Testing for exactly one output ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetInputCube("FROM2");
    p.SetOutputCube("TO");
    p.SetBoxcarSize (3,3);
    cout << "Testing for too many input cubes ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO");
    p.SetOutputCube("TO2");
    p.SetBoxcarSize (3,3);
    cout << "Testing for too many output cubes ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO",1,1,1);
    p.SetBoxcarSize (3,3);
    cout << "Testing for lines mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO",1,126,1);
    p.SetBoxcarSize (3,3);
    cout << "Testing for samples mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }
  
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO",126,126,1);
    p.SetBoxcarSize (3,3);
    cout << "Testing for bands mismatch ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }
  
  try {
    p.SetInputCube("FROM");
    p.SetOutputCube("TO");
    cout << "Testing for boxcar size not set ..." << endl;
    p.StartProcess(oneInAndOut);
  }
  catch (Isis::iException &e) {
    e.Report(false);
    p.EndProcess();
    cout << endl;
  }

  Isis::Cube cube;
  cube.Open("/tmp/isisProcessByBoxcar_01"); 
  cube.Close(true);
  cube.Open("/tmp/isisProcessByBoxcar_02"); 
  cube.Close(true);

}

void oneInAndOut (Isis::Buffer &ib, double &ob) {
	static bool firstTime = true;
  if (firstTime) {
    firstTime = false;
    cout << endl;
    cout << "Testing one input and output cube ... " << endl;
    cout << "Boxcar Samples:  " << ib.SampleDimension() << endl;
    cout << "Boxcar Lines:    " << ib.LineDimension() << endl;
    cout << "Boxcar Bands:    " << ib.BandDimension() << endl;
    cout << endl;
  }
  
  if (ib.Sample() < 1) {
    cout << "Top Left Sample:  " << ib.Sample() 
         << ", Top Left Line:  " << ib.Line() 
         << ", Top Left Band:  " << ib.Band() << endl;
  }
}

